/*
    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999-2002 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


    rtsyn_winmm.c
        Copyright (c) 2003 Keishi Suenaga <s_keishi@mutt.freemail.ne.jp>

    I referenced following sources.
        alsaseq_c.c - ALSA sequencer server interface
            Copyright (c) 2000  Takashi Iwai <tiwai@suse.de>
        readmidi.c

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */


#ifdef IA_WINSYN

#ifdef __POCC__
#include <sys/types.h>
#endif //for off_t

#ifdef __W32__
#include <windows.h>
#include <mmsystem.h>
#endif

#include "timidity.h"
#include "common.h"
#include "controls.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "recache.h"
#include "output.h"
#include "aq.h"
#include "timer.h"

#include "rtsyn.h"
#include "rtsyn_internal.h"
#ifdef USE_TWSYN_BRIDGE
#include "twsyn_bridge_common.h"
#include "twsyn_bridge_host.h"
#endif

unsigned int portID[MAX_PORT];
char rtsyn_portlist[32][80];
int rtsyn_nportlist;

#define MAX_EXBUF 20
#define BUFF_SIZE 512

UINT InNum;
HMIDIIN  hMidiIn[MAX_PORT];
HMIDIOUT hMidiOut[MAX_PORT];
MIDIHDR *IMidiHdr[MAX_PORT][MAX_EXBUF];

char sIMidiHdr[MAX_PORT][MAX_EXBUF][sizeof(MIDIHDR)];
char sImidiHdr_data[MAX_PORT][MAX_EXBUF][BUFF_SIZE];

struct evbuf_t{
	UINT wMsg;
	DWORD_PTR	dwInstance;
	DWORD_PTR	dwParam1;
	DWORD_PTR	dwParam2;
};
#define EVBUFF_SIZE 512
struct evbuf_t evbuf[EVBUFF_SIZE];
UINT  evbwpoint=0;
UINT  evbrpoint=0;
UINT evbsysexpoint;
UINT  mvbuse=0;

CRITICAL_SECTION mim_section;

double mim_start_time;

void CALLBACK MidiInProc(HMIDIIN hMidiInL, UINT wMsg, DWORD_PTR dwInstance,	DWORD_PTR dwParam1, DWORD_PTR dwParam2);

static void rtsyn_ws_get_port_list(void);
static int  rtsyn_ws_synth_start(void);
static void rtsyn_ws_synth_stop(void);
static int  rtsyn_ws_play_some_data(void);
static void rtsyn_ws_midiports_close(void);
static int  rtsyn_ws_buf_check(void);

void rtsyn_ws_setup(void)
{
    rtsyn.id_character    = 'W';
    rtsyn.get_port_list   = rtsyn_ws_get_port_list;
    rtsyn.synth_start     = rtsyn_ws_synth_start;
    rtsyn.synth_stop      = rtsyn_ws_synth_stop;
    rtsyn.play_some_data  = rtsyn_ws_play_some_data;
    rtsyn.midiports_close = rtsyn_ws_midiports_close;
    rtsyn.buf_check       = rtsyn_ws_buf_check;
}

void rtsyn_ws_get_port_list(){
	int i;
	MIDIINCAPS InCaps;

#ifdef USE_TWSYN_BRIDGE
	if(opt_use_twsyn_bridge){
		InNum = get_bridge_midi_devs();		
		for (i=1;i <=InNum && i<=32;i++){
			sprintf(rtsyn_portlist[i-1],"%d:%s",i,(LPSTR)get_bridge_midi_dev_name(i));
		}
		rtsyn_nportlist=i-1;
	}else
#endif
	{
		InNum = midiInGetNumDevs();
		for (i=1;i <=InNum && i<=32;i++){
			midiInGetDevCaps(i-1,(LPMIDIINCAPSA) &InCaps,sizeof(InCaps));
			sprintf(rtsyn_portlist[i-1],"%d:%s",i,(LPSTR)InCaps.szPname);
		}
		rtsyn_nportlist=i-1;
	}
}

int rtsyn_ws_synth_start(){
	int i;
	UINT port;

#ifdef __W32__
	DWORD processPriority;
	processPriority = GetPriorityClass(GetCurrentProcess());
#endif

	
	port=0;
	evbwpoint=0;
	evbrpoint=0;
	mvbuse=0;
	sleep(1);
#ifdef USE_TWSYN_BRIDGE
	if(opt_use_twsyn_bridge){
		open_bridge_midi_dev(rtsyn_portnumber, portID);
	}else
#endif
	{
		for(port=0;port<rtsyn_portnumber;port++){
			for (i=0;i<MAX_EXBUF;i++){
				IMidiHdr[port][i] = (MIDIHDR *)sIMidiHdr[port][i];
				memset(IMidiHdr[port][i],0,sizeof(MIDIHDR));
				IMidiHdr[port][i]->lpData = sImidiHdr_data[port][i];
				memset((IMidiHdr[port][i]->lpData),0,BUFF_SIZE);
				IMidiHdr[port][i]->dwBufferLength = BUFF_SIZE;
			}
		}

		for(port=0;port<rtsyn_portnumber;port++){
			midiInOpen(&hMidiIn[port],portID[port],(DWORD_PTR)MidiInProc,(DWORD_PTR)port,CALLBACK_FUNCTION);
			for (i=0;i<MAX_EXBUF;i++){
				midiInUnprepareHeader(hMidiIn[port],IMidiHdr[port][i],sizeof(MIDIHDR));
				midiInPrepareHeader(hMidiIn[port],IMidiHdr[port][i],sizeof(MIDIHDR));
				midiInAddBuffer(hMidiIn[port],IMidiHdr[port][i],sizeof(MIDIHDR));
			}
		}

#ifdef __W32__
		// HACK:midiInOpen()でリセットされてしまうため、再設定
		SetPriorityClass(GetCurrentProcess(), processPriority);
#endif
		for(port=0;port<rtsyn_portnumber;port++){
			if(MMSYSERR_NOERROR !=midiInStart(hMidiIn[port])){
				int i;
				for(i=0;i<port;i++){
					midiInStop(hMidiIn[i]);
					midiInReset(hMidiIn[i]);
					midiInClose(hMidiIn[i]);
				}
				goto winmmerror;
			}
		}
	}
	mim_start_time = get_current_calender_time();
	InitializeCriticalSection(&mim_section);
	return ~0;

winmmerror:
	ctl->cmsg(  CMSG_ERROR, VERB_NORMAL, "midiInStarterror\n" );
	return 0;
}

void rtsyn_ws_synth_stop(){
	rtsyn_stop_playing();
	//	play_mode->close_output();	
	rtsyn_midiports_close();
	DeleteCriticalSection(&mim_section);
	return;
}
void rtsyn_ws_midiports_close(void){
	UINT port;

#ifdef USE_TWSYN_BRIDGE
	if(opt_use_twsyn_bridge)
		close_bridge_midi_dev();
	else
#endif	
	for(port=0;port<rtsyn_portnumber;port++){
		if( MMSYSERR_NOERROR!=midiInStop(hMidiIn[port]) )
			ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIDI Stop Error\n");
		if( MMSYSERR_NOERROR!=midiInReset(hMidiIn[port]) ) 
			ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIDI Rest Error\n");
		if( MMSYSERR_NOERROR!=midiInClose(hMidiIn[port]) ) 
			ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIDI Close Error\n");
	}
}

int rtsyn_ws_buf_check(void){
	int retval;
	EnterCriticalSection(&mim_section);
	retval = (evbrpoint != evbwpoint) ? ~0 :  0;
	LeaveCriticalSection(&mim_section);
	return retval;
}

int rtsyn_ws_play_some_data(void){
	UINT wMsg;
	DWORD_PTR	dwInstance;
	DWORD_PTR	dwParam1;
	DWORD_PTR	dwParam2;
	DWORD	timestamp;
	MidiEvent ev;
	MidiEvent evm[260];
	int port;
	UINT evbpoint;
	MIDIHDR *IIMidiHdr;
	int exlen;
	char *sysexbuffer;
	int ne,i,j,chk,played;
		
	played=0;
		if( !rtsyn_buf_check() ){ 
			played=~0;
			return played;
		}
		do{
			EnterCriticalSection(&mim_section);
			evbpoint=evbrpoint;
			if (++evbrpoint >= EVBUFF_SIZE)
					evbrpoint -= EVBUFF_SIZE;

			wMsg=evbuf[evbpoint].wMsg;
			dwInstance=evbuf[evbpoint].dwInstance;
			dwParam1=evbuf[evbpoint].dwParam1;
			dwParam2=evbuf[evbpoint].dwParam2;
			LeaveCriticalSection(&mim_section);
		    
			port=(UINT)dwInstance;
			switch (wMsg) {
			case MIM_DATA:
				rtsyn_play_one_data (port, dwParam1, mim_start_time+(double)dwParam2 * DIV_1000);
				break;
			case MIM_LONGDATA:
#ifdef USE_TWSYN_BRIDGE
				if(opt_use_twsyn_bridge){
					exlen = get_bridge_mim_databytes((int)dwParam1);
					sysexbuffer = get_bridge_mim_longdata((int)dwParam1);
					rtsyn_play_one_sysex(sysexbuffer, exlen, mim_start_time + (double)dwParam2 * DIV_1000);
				}else
#endif
				{
					IIMidiHdr = (MIDIHDR *) dwParam1;
					exlen=(int)IIMidiHdr->dwBytesRecorded;
					sysexbuffer=IIMidiHdr->lpData;
					rtsyn_play_one_sysex (sysexbuffer,exlen, mim_start_time+(double)dwParam2 * DIV_1000);
					if (MMSYSERR_NOERROR != midiInUnprepareHeader(
							hMidiIn[port], IIMidiHdr, sizeof(MIDIHDR)))
						ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"error1\n");
					if (MMSYSERR_NOERROR != midiInPrepareHeader(
							hMidiIn[port], IIMidiHdr, sizeof(MIDIHDR)))
						ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"error5\n");
					if (MMSYSERR_NOERROR != midiInAddBuffer(
							hMidiIn[port], IIMidiHdr, sizeof(MIDIHDR)))
						ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"error6\n");
				}
				break;
			}
		}while(rtsyn_buf_check());	
	return played;
}

void CALLBACK MidiInProc(HMIDIIN hMidiInL, UINT wMsg, DWORD_PTR dwInstance,
		DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	UINT evbpoint;
	UINT port;
	
	port=(UINT)dwInstance;
	switch (wMsg) {
	case MIM_DATA:
	case MIM_LONGDATA:
		EnterCriticalSection(&mim_section);
		evbpoint = evbwpoint;
		if (++evbwpoint >= EVBUFF_SIZE)
			evbwpoint -= EVBUFF_SIZE;
		evbuf[evbpoint].wMsg = wMsg;
		evbuf[evbpoint].dwInstance = dwInstance;
		evbuf[evbpoint].dwParam1 = dwParam1;
		evbuf[evbpoint].dwParam2 = dwParam2;
		LeaveCriticalSection(&mim_section);
		break;
	case MIM_OPEN:
//		ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIM_OPEN\n");
		break;
	case MIM_CLOSE:
//		ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIM_CLOSE\n");
		break;
	case MIM_LONGERROR:
		ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIM_LONGERROR\n");
		break;
	case MIM_ERROR:
		ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIM_ERROR\n");
		break;
	case MIM_MOREDATA:
		ctl->cmsg(  CMSG_ERROR, VERB_NORMAL,"MIM_MOREDATA\n");
		break;
	}
}

#endif /* IA_WINSYN */

