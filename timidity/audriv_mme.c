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
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <stdio.h>
#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <mme/mme_api.h>
#include "timidity.h"
#include "aenc.h"
#include "audriv.h"
#include "timer.h"

#define DATA_BLOCK_SIZE 2048
#define DATA_BLOCK_NUM  48
#define AUDIO_CLOSE_ID -1
#define IS_AUDIO_PLAY_OPEN  (play_wave_format && play_wave_format->hWave != AUDIO_CLOSE_ID)
#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

struct wave_format_t
{
    HWAVE hWave;
    PCMWAVEFORMAT format;
    WAVEHDR waveHdr;
    UINT DevID;
};
static struct wave_format_t *play_wave_format = NULL;
static MMTIME *wave_time_info = NULL;
static LPDWORD lpdword_buff = NULL;
static LPWAVEOUTCAPS wave_out_caps = NULL;

#define AUDRIV_WRITE 1

struct data_block_t
{
    LPSTR data;
    int data_start;
    int data_size;
    int blockno;
    int in_use;			/* 0, AUDRIV_WRITE */
    struct data_block_t *next;
};

static struct data_block_t all_data_block[DATA_BLOCK_NUM];
static struct data_block_t *free_data_block;
static int data_block_length;	/* used data block length */
void (* audriv_error_handler)(const char *errmsg) = NULL;

static Bool audio_write_noblocking = False;
static double play_start_time;
static long play_counter, reset_samples;
static int output_port = AUDRIV_OUTPUT_SPEAKER;

char audriv_errmsg[BUFSIZ];

static const char *mme_mmsyserr_code_string[] =
{
    "no error",
    "unspecified error",
    "device ID out of range",
    "driver failed enable",
    "device already allocated",
    "device handle is invalid",
    "no device driver present",
    "memory allocation error",
    "function isn't supported",
    "error value out of range",
    "invalid flag passed",
    "invalid parameter passed",
    "handle being used",
    "simultaneously on another",
    "thread (eg callback)",
    "Specified alias not found in WIN.INI",
};

static const char *mme_waverr_code_sring[] =
{
    "unsupported wave format",
    "still something playing",
    "header not prepared",
    "device is synchronous",
    "Device in use",
    "Device in use",
};

static const char *mme_midierr_code_sring[] =
{
    "header not prepared",
    "still something playing",
    "no current map",
    "hardware is still busy",
    "port no longer connected",
    "invalid setup",
};

static const char *mme_error_code_string(MMRESULT err_code)
{
    if(err_code >= TIMERR_BASE)
    {
	switch(err_code)
	{
	  case TIMERR_NOCANDO:
	    return "request not completed";
	  case TIMERR_STRUCT:
	    return "time struct size";
	}
	return "";
    }
    if(err_code >= MIDIERR_BASE)
    {
	err_code -= MIDIERR_BASE;
	if(err_code > ARRAY_SIZE(mme_midierr_code_sring))
	    return "";
	return mme_midierr_code_sring[err_code];
    }
    if(err_code >= WAVERR_BASE)
    {
	err_code -= WAVERR_BASE;
	if(err_code > ARRAY_SIZE(mme_waverr_code_sring))
	    return "";
	return mme_waverr_code_sring[err_code];
    }
    if(err_code > ARRAY_SIZE(mme_mmsyserr_code_string))
	return "";
    return mme_mmsyserr_code_string[err_code];
}

static void audriv_err(const char *msg)
{
    strncpy(audriv_errmsg, msg, sizeof(audriv_errmsg) - 1);
    if(audriv_error_handler != NULL)
	audriv_error_handler(audriv_errmsg);
}

static struct data_block_t *new_data_block()
{
    struct data_block_t *p;
    if(free_data_block == NULL)
	return NULL;
    p = free_data_block;
    free_data_block = free_data_block->next;
    data_block_length++;
    p->next = NULL;
    p->data_start = 0;
    p->data_size = DATA_BLOCK_SIZE;
    return p;
}

static void reuse_data_block(int blockno)
{
    if(all_data_block[blockno].in_use)
    {
	all_data_block[blockno].next = free_data_block;
	all_data_block[blockno].in_use = 0;
	free_data_block = all_data_block + blockno;
	data_block_length--;
    }
}

static void reset_data_block(void)
{
    int i;

    all_data_block[0].blockno = 0;
    all_data_block[0].next = all_data_block + 1;
    for(i = 1; i < DATA_BLOCK_NUM - 1; i++)
    {
	all_data_block[i].blockno = i;
	all_data_block[i].in_use = 0;
	all_data_block[i].next = all_data_block + i + 1;
    }
    all_data_block[i].blockno = i;
    all_data_block[i].in_use = 0;
    all_data_block[i].next = NULL;
    free_data_block = all_data_block;
    data_block_length = 0;
}

Bool audriv_setup_audio(void)
/* �I�[�f�B�I�̏��������s���܂��D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    int i;

    play_wave_format = (struct wave_format_t *)
	mmeAllocMem(sizeof(struct wave_format_t));
    if(play_wave_format == NULL)
    {
	audriv_err(mme_error_code_string(MMSYSERR_NOMEM));
	return False;
    }
    play_wave_format->DevID = 0;

    wave_time_info = (MMTIME *)mmeAllocMem(sizeof(MMTIME));
    if(wave_time_info == NULL)
    {
	audriv_free_audio();
	audriv_err(mme_error_code_string(MMSYSERR_NOMEM));
	return False;
    }

    lpdword_buff = (LPDWORD)mmeAllocMem(sizeof(DWORD));
    if(lpdword_buff == NULL)
    {
	audriv_free_audio();
	audriv_err(mme_error_code_string(MMSYSERR_NOMEM));
	return False;
    }

    wave_out_caps = (LPWAVEOUTCAPS)mmeAllocMem(sizeof(WAVEOUTCAPS));
    if(wave_out_caps == NULL)
    {
	audriv_free_audio();
	audriv_err(mme_error_code_string(MMSYSERR_NOMEM));
	return False;
    }

    all_data_block[0].data = mmeAllocBuffer(DATA_BLOCK_NUM * DATA_BLOCK_SIZE);

#ifdef DEBUG
    fprintf(stderr, "%d shared audio buffer memory is allocated.\n",
	   DATA_BLOCK_NUM * DATA_BLOCK_SIZE);
#endif /* DEBUG */

    if(all_data_block[0].data == NULL)
    {
	audriv_free_audio();
	audriv_err(mme_error_code_string(MMSYSERR_NOMEM));
	return False;
    }
    for(i = 1; i < DATA_BLOCK_NUM; i++)
	all_data_block[i].data = all_data_block[0].data + i * DATA_BLOCK_SIZE;
    reset_data_block();

    play_wave_format->hWave = AUDIO_CLOSE_ID;
    play_wave_format->format.wf.wFormatTag = WAVE_FORMAT_MULAW;
    play_wave_format->format.wf.nChannels = 1;
    play_wave_format->format.wf.nSamplesPerSec = 8000;
    play_wave_format->format.wf.nAvgBytesPerSec = 8000;
    play_wave_format->format.wf.nBlockAlign = 1;
    play_wave_format->format.wBitsPerSample = 8;

    audriv_play_open();
    audriv_play_close();
    return True;
}

void audriv_free_audio(void)
/* audio �̌㏈�����s���܂��D
 */
{
    audriv_play_close();

    if(all_data_block[0].data)
	mmeFreeBuffer(all_data_block[0].data);
    all_data_block[0].data = NULL;

    if(play_wave_format)
	mmeFreeMem(play_wave_format);
    play_wave_format = NULL;

    if(wave_time_info)
	mmeFreeMem(wave_time_info);
    wave_time_info = NULL;

    if(lpdword_buff)
	mmeFreeMem(lpdword_buff);
    lpdword_buff = NULL;

    if(wave_out_caps)
	mmeFreeMem(wave_out_caps);
    wave_out_caps = NULL;
}

static const char *callback_message_string(UINT msg)
{
    switch(msg)
    {
      case WOM_OPEN:
	return "WOM_OPEN";
      case WOM_CLOSE:
	return "WOM_CLOSE";
      case WOM_DONE:
	return "WOM_DONE";
      case WIM_OPEN:
	return "WIM_OPEN";
      case WIM_CLOSE:
	return "WIM_CLOSE";
      case WIM_DATA:
	return "WIM_DATA";
      default:
	return "Unknown";
    }
}

static void audriv_callback_mme(HANDLE Hwave, UINT wMsg, DWORD dwInstance,
				LPARAM lParam1, LPARAM lParam2)
{
#ifdef DEBUGALL
    fprintf(stderr, "audriv_callback_mme: mMsg=%d (%s)\n",
	   wMsg, callback_message_string(wMsg));
#endif

    if(wMsg == WOM_DONE)
    {
	WAVEHDR *wp = (WAVEHDR *)lParam1;
#ifdef DEBUGALL
	fprintf(stderr, "output wp->dwUser = %d\n", wp->dwUser);
#endif
	reuse_data_block(wp->dwUser);
    }
}

#ifdef DEBUG
static const char *ProductID_names(WORD id)
{
    switch(id)
    {
      case MM_MIDI_MAPPER: return "MIDI Mapper";
      case MM_WAVE_MAPPER: return "Wave Mapper";
      case MM_SNDBLST_MIDIOUT: return "Sound Blaster MIDI output port";
      case MM_SNDBLST_MIDIIN: return "Sound Blaster MIDI input port";
      case MM_SNDBLST_SYNTH: return "Sound Blaster internal synthesizer";
      case MM_SNDBLST_WAVEOUT: return "Sound Blaster waveform output";
      case MM_SNDBLST_WAVEIN: return "Sound Blaster waveform input";
      case MM_ADLIB: return "Ad Lib-compatible synthesizer";
      case MM_MPU401_MIDIOUT: return "MPU401-compatible MIDI output port";
      case MM_MPU401_MIDIIN: return "MPU401-compatible MIDI input port";
      case MM_PC_JOYSTICK: return "Joystick adapter";

#ifdef MM_DIGITAL_BBA
      case MM_DIGITAL_BBA: return "DEC BaseBoard Audio for ALPHA-AXP";
#endif /* MM_DIGITAL_BBA */

#ifdef MM_DIGITAL_J300AUDIO
      case MM_DIGITAL_J300AUDIO: return "DEC J300 TC option card";
#endif /* MM_DIGITAL_J300AUDIO */

#ifdef MM_DIGITAL_MSB
      case MM_DIGITAL_MSB: return "DEC Microsoft Sound Board compatible card";
#endif /* MM_DIGITAL_MSB */

#ifdef MM_DIGITAL_IMAADPCM
      case MM_DIGITAL_IMAADPCM: return "DEC IMA ADPCM ACM driver";
#endif /* MM_DIGITAL_IMAADPCM */
    }
    return "Unknown";
}
#endif /* DEBUG */

Bool audriv_play_open(void)
{
    MMRESULT status;

    if(IS_AUDIO_PLAY_OPEN)
	return True;

    play_counter = 0;
    reset_samples = 0;

    play_wave_format->hWave = 0;
    status = waveOutOpen(&play_wave_format->hWave,
			 WAVE_MAPPER,
			 &play_wave_format->format.wf,
			 audriv_callback_mme,
			 NULL,
			 CALLBACK_FUNCTION | WAVE_OPEN_SHAREABLE);

    if(status != MMSYSERR_NOERROR)
    {
	audriv_err(mme_error_code_string(status));
	play_wave_format->hWave = AUDIO_CLOSE_ID;
	return False;
    }

    status = waveOutGetID(play_wave_format->hWave,
			  &play_wave_format->DevID);
    if(status != MMSYSERR_NOERROR)
    {
	audriv_err(mme_error_code_string(status));
	audriv_play_close();
	return False;
    }

    status = waveOutGetDevCaps(play_wave_format->DevID,
			       wave_out_caps, sizeof(WAVEOUTCAPS));
    if(status != MMSYSERR_NOERROR)
    {
	audriv_err(mme_error_code_string(status));
	audriv_play_close();
	return False;
    }

#ifdef DEBUG
    fprintf(stderr, "Play Device ID: %d\n", play_wave_format->DevID);
#ifdef MM_MICROSOFT
    if(wave_out_caps->wMid == MM_MICROSOFT)
	fputs("Manufacture: Microsoft Corp.", stderr);
#endif /* MM_MICROSOFT */
#ifdef MM_DIGITAL
    if(wave_out_caps->wMid == MM_DIGITAL)
	fputs("Manufacture: Digital Eq. Corp.", stderr);
#endif /* MM_DIGITAL */

    fprintf(stderr, "Product: %s\n", ProductID_names(wave_out_caps->wPid));
    fprintf(stderr, "Version of the driver: %d\n", wave_out_caps->vDriverVersion);
    fprintf(stderr, "Product name: %s\n", wave_out_caps->szPname);
    fprintf(stderr, "Formats supported: 0x%x\n", wave_out_caps->dwFormats);
    fprintf(stderr, "number of sources supported: 0x%x\n", wave_out_caps->wChannels);
    fprintf(stderr, "functionality supported by driver: 0x%x\n",
	   wave_out_caps->dwSupport);
#endif

    mmeProcessCallbacks();

    return True;
}

void audriv_play_close(void)
{
    if(!IS_AUDIO_PLAY_OPEN)
	return;
    waveOutClose(play_wave_format->hWave);
    mmeProcessCallbacks();
    play_wave_format->hWave = AUDIO_CLOSE_ID;
}

static long calculate_play_samples(void)
{
    double es;

    if(play_counter <= 0)
	return 0;

    es = (double)(play_wave_format->format.wf.nSamplesPerSec
		  * play_wave_format->format.wf.nBlockAlign)
	* (get_current_calender_time() - play_start_time);
    if(es > play_counter)
	return play_counter / play_wave_format->format.wf.nBlockAlign;
    return (long)(es / play_wave_format->format.wf.nBlockAlign);
}

long audriv_play_stop(void)
/* ���t�𑦍��ɒ�~���C��~���O�̃T���v������Ԃ��܂��D
 * audriv_play_stop() �̌Ăяo���ɂ���āCaudio �͕��܂��D
 * audio �����ɕ��Ă���ꍇ�� audriv_play_stop() ���Ăяo�����ꍇ�� 0 ��
 * �Ԃ��܂��D
 * �G���[�̏ꍇ�� -1 ��Ԃ��܂��D
 */
{
    MMRESULT status;

    if(!IS_AUDIO_PLAY_OPEN)
	return 0;

    status = waveOutReset(play_wave_format->hWave);
    if(status != MMSYSERR_NOERROR)
    {
	audriv_err(mme_error_code_string(status));
	return -1;
    }

    waveOutClose(play_wave_format->hWave);
    mmeProcessCallbacks();
    play_wave_format->hWave = AUDIO_CLOSE_ID;

    return calculate_play_samples();
}

Bool audriv_is_play_open(void)
/* audio �����t�ŃI�[�v������Ă���ꍇ�� True,
 * ���Ă���ꍇ�� False ��Ԃ��܂��D
 */
{
    return IS_AUDIO_PLAY_OPEN;
}

Bool audriv_set_play_volume(int volume)
/* ���t���ʂ� 0 ? 255 �͈͓̔��Őݒ肵�܂��D0 �͖����C255 �͍ő剹�ʁD
 * 0 ������ 0�C255 �𒴂���l�� 255 �ɓ����D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    unsigned short left, right;
    DWORD vol;
    MMRESULT status;

    if(volume < 0)
	volume = 0;
    else if(volume > 255)
	volume = 255;

    left = right = ((unsigned)volume) * 257;
    vol = ((DWORD)left | (((DWORD)right) << 16));

    status = waveOutSetVolume(play_wave_format->DevID, vol);
    if(status != MMSYSERR_NOERROR)
    {
	audriv_err(mme_error_code_string(status));
	return False;
    }

    return True;
}

int audriv_get_play_volume(void)
/* ���t���ʂ� 0 ? 255 ���œ��܂��D0 �͖����C255 �͍ő剹�ʁD
 * ���s����� -1 ��Ԃ��C�����łȂ��ꍇ�� 0 ? 255 ���̉��ʂ�Ԃ��܂��D
 */
{
    MMRESULT status;
    unsigned short left, right;

    status = waveOutGetVolume(play_wave_format->DevID, lpdword_buff);
    if(status != MMSYSERR_NOERROR)
    {
	audriv_err(mme_error_code_string(status));
	return -1;
    }

    left  = (*lpdword_buff & 0xffff) / 257;
    right = ((UINT)*lpdword_buff >> 16) / 257;

    return (int)((left + right) / 2);
}

/* FIXME */
Bool audriv_set_play_output(int port)
/* audio �̏o�͐� port ���w�肵�܂��Dport �ɂ͈ȉ��̂ǂꂩ���w�肵�܂��D
 *
 *     AUDRIV_OUTPUT_SPEAKER	�X�s�[�J�ɏo�́D
 *     AUDRIV_OUTPUT_HEADPHONE	�w�b�h�z���ɏo�́D
 *     AUDRIV_OUTPUT_LINE_OUT	���C���A�E�g�ɏo�́D
 *
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    switch(port)
    {
      case AUDRIV_OUTPUT_SPEAKER:
      case AUDRIV_OUTPUT_HEADPHONE:
      case AUDRIV_OUTPUT_LINE_OUT:
	output_port = port;
	break;
      default:
	audriv_err("�w��O�̏o�̓|�[�g���w�肳��܂����D");
	return False;
    }
    return True;
}

/* FIXME */
int audriv_get_play_output(void)
/* audio �̏o�͐� port �𓾂܂��D
 * ���s����� -1 ��Ԃ��C��������ƈȉ��̂����ꂩ�̒l��Ԃ��܂��D
 *
 *     AUDRIV_OUTPUT_SPEAKER	�X�s�[�J�ɏo�́D
 *     AUDRIV_OUTPUT_HEADPHONE	�w�b�h�z���ɏo�́D
 *     AUDRIV_OUTPUT_LINE_OUT	���C���A�E�g�ɏo�́D
 *
 */
{
    return output_port;
}

int audriv_write(char *buff, int n)
/* audio �� buff �� n �o�C�g���������݂܂��D
 * audriv_set_noblock_write() �Ŕ�u���b�N�E���[�h���ݒ肳�ꂽ
 * �ꍇ�́C���̊֐��̌Ăяo���͑����ɏ������Ԃ�܂��D
 * �Ԃ�l�͎��ۂɗ������܂ꂽ�o�C�g���ł���C��u���b�N�E���[�h���ݒ�
 * ����Ă���ꍇ�́C���� n ��菭�Ȃ��ꍇ������܂��D
 * ���s����� -1 ��Ԃ��C��������ƁC���ۂɗ������܂ꂽ�o�C�g����Ԃ��܂��D
 */
{
    struct data_block_t *block;
    int len, total, loop_cnt;
    MMRESULT status;

    total = 0;
    loop_cnt = 0;
    while(n > 0)
    {
	mmeProcessCallbacks();

	if((block = new_data_block()) == NULL)
	{
	    if(audio_write_noblocking)
		return total;
	    mmeWaitForCallbacks (); /* block so we don't hog 100% of the CPU */
	    continue;
	}
	block->in_use = AUDRIV_WRITE;
	if(n <= DATA_BLOCK_SIZE)
	    len = n;
	else
	    len = DATA_BLOCK_SIZE;
	memcpy(block->data, buff + total, len);
	play_wave_format->waveHdr.dwBufferLength = len;
	play_wave_format->waveHdr.lpData = block->data;
	play_wave_format->waveHdr.dwUser = block->blockno;
	status = waveOutWrite(play_wave_format->hWave,
			      &play_wave_format->waveHdr,
			      sizeof(WAVEHDR));
	if(status != MMSYSERR_NOERROR)
	{
	    audriv_err(mme_error_code_string(status));
	    return -1;
	}

	if(audriv_play_active() == 0)
	{
	    reset_samples +=
		play_counter / play_wave_format->format.wf.nBlockAlign;
	    play_counter = 0;
	}
	if(play_counter == 0)
	    play_start_time = get_current_calender_time();
	play_counter += len;
	total += len;

	n -= len;
	loop_cnt++;
	if(audio_write_noblocking && loop_cnt >= DATA_BLOCK_NUM/2)
	    break;
    }
    return total;
}

Bool audriv_set_noblock_write(Bool noblock)
/* noblock �� True ���w�肷��ƁCaudriv_write() �Ăяo���Ńu���b�N���܂���D
 * False ���w�肷��ƁC�f�t�H���g�̏�Ԃɖ߂��܂��D
 * �����ɐ�������� True ���C���s����� False ��Ԃ��܂��D
 */
{
    audio_write_noblocking = noblock;
    return True;
}

int audriv_play_active(void)
/* ���t���Ȃ� 1�C���t���łȂ��Ȃ� 0, �G���[�Ȃ� -1 ��Ԃ��܂��D
 */
{
    double es;

    if(play_counter <= 0)
	return 0;
    es = (double)(play_wave_format->format.wf.nSamplesPerSec
		  * play_wave_format->format.wf.nBlockAlign)
	* (get_current_calender_time() - play_start_time);
    if(es > play_counter)
	return 0;
    return 1;
}

long audriv_play_samples(void)
/* ���݉��t���̃T���v���ʒu��Ԃ��܂��D
 */
{
#if 1 /* audriv_play_samples() */
    return reset_samples + calculate_play_samples();
#else
    MMRESULT status;
    int sample;

    wave_time_info->wType = TIME_SAMPLES;
    status = waveOutGetPosition(play_wave_format->hWave,
				wave_time_info,
				sizeof(MMTIME));
    if(status != MMSYSERR_NOERROR)
    {
	audriv_err(mme_error_code_string(status));
	return -1;
    }
    sample = (int)wave_time_info->u.sample;
    if(sample < 0)
	return 0;
    return (long)sample;
#endif
}

long audriv_get_filled(void)
/* �I�[�f�B�I�o�b�t�@���̃o�C�g����Ԃ��܂��B
 * �G���[�̏ꍇ�� -1 ��Ԃ��܂��B
 */
{
    double es;

    if(play_counter <= 0)
	return 0;
    es = (double)(play_wave_format->format.wf.nSamplesPerSec *
		  play_wave_format->format.wf.nBlockAlign)
	* (get_current_calender_time() - play_start_time);
    if(es > play_counter)
	return 0;
    return play_counter - (long)es;
}

const long *audriv_available_encodings(int *n_ret)
/* �}�V�����T�|�[�g���Ă��邷�ׂĂ̕��������X�g��Ԃ��܂��Dn_ret �ɂ�
 * ���̎�ނ̐����C�Ԃ���܂��D������������킷�萔�l��
 * aenc.h ���ɒ�`����Ă��� enum audio_encoding_types �̒l�ł��D
 * �Ԃ�l�� free ���Ă͂Ȃ�܂���D
 */
{
    static const long encoding_list[] =
    {
	AENC_UNSIGBYTE,	/* WAVE_FORMAT_PCM */
	AENC_G711_ULAW,	/* WAVE_FORMAT_MULAW */
	AENC_SIGWORDL,	/* WAVE_FORMAT_PCM */
    };

    *n_ret = 3;
    return encoding_list;
}

const long *audriv_available_sample_rates(int *n_ret)
/* �}�V�����T�|�[�g���Ă��邷�ׂẴT���v�����[�g�̃��X�g��Ԃ��܂��D
 * n_ret �ɂ͂��̎�ނ̐����C�Ԃ���܂��D
 * �Ԃ�l�� free ���Ă͂Ȃ�܂���D
 */
{
    static const long sample_rates[] =
    {
	5512, 6615, 8000, 9600, 11025, 16000, 18900, 22050, 32000, 37800,
	44100, 48000
	};
    *n_ret = 12;
    return sample_rates;
}

const long *audriv_available_channels(int *n_ret)
/* �}�V�����T�|�[�g���Ă��邷�ׂẴ`���l�����̃��X�g��Ԃ��܂��D
 * n_ret �ɂ͂��̎�ނ̐����C�Ԃ���܂��D
 * �Ԃ�l�� free ���Ă͂Ȃ�܂���D
 */
{
    static const long channels[] = {1, 2};
    *n_ret = 2;
    return channels;
}

static void set_wave_format_encoding(PCMWAVEFORMAT *format, long encoding)
{
    switch(encoding)
    {
      case AENC_UNSIGBYTE:
	format->wf.wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 8;
	break;
      case AENC_G711_ULAW:
	format->wf.wFormatTag = WAVE_FORMAT_MULAW;
	format->wBitsPerSample = 8;
	break;
      case AENC_G711_ALAW:
	format->wf.wFormatTag = WAVE_FORMAT_ALAW;
	format->wBitsPerSample = 8;
	break;
      case AENC_SIGWORDL:
	format->wf.wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	break;
      case AENC_UNSIGWORDL:
	format->wf.wFormatTag = WAVE_FORMAT_PCM;
	format->wBitsPerSample = 16;
	break;
    }
    format->wf.nBlockAlign =
	format->wf.nChannels * (format->wBitsPerSample / 8);
}

Bool audriv_set_play_encoding(long encoding)
/* audio ���t���̕������������w�肵�܂��D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    int i, n;
    const long *enc;

    enc = audriv_available_encodings(&n);
    for(i = 0; i < n; i++)
	if(enc[i] == encoding)
	{
	    set_wave_format_encoding(&play_wave_format->format, encoding);
	    return True;
	}
    return False;
}

Bool audriv_set_play_sample_rate(long sample_rate)
/* audio ���t���̃T���v�����[�g���w�肵�܂��D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    int i, n;
    const long *r;

    r = audriv_available_sample_rates(&n);
    for(i = 0; i < n; i++)
	if(r[i] == sample_rate)
	{
	    play_wave_format->format.wf.nSamplesPerSec = sample_rate;
	    play_wave_format->format.wf.nAvgBytesPerSec =
		play_wave_format->format.wf.nSamplesPerSec *
		play_wave_format->format.wf.nBlockAlign;
	    return True;
	}
    return False;
}

Bool audriv_set_play_channels(long channels)
/* ���t�p�̃`���l������ݒ肵�܂��D
 * ���s����� False ��Ԃ��C��������� True ��Ԃ��܂��D
 */
{
    int i, n;
    const long *c = audriv_available_channels(&n);

    for(i = 0; i < n; i++)
	if(channels == c[i])
	{
	    play_wave_format->format.wf.nChannels = channels;
	    play_wave_format->format.wf.nBlockAlign =
		channels * (play_wave_format->format.wBitsPerSample / 8);
	    play_wave_format->format.wf.nAvgBytesPerSec =
		play_wave_format->format.wf.nSamplesPerSec *
		play_wave_format->format.wf.nBlockAlign;
	    return True;
	}
    return False;
}

void audriv_wait_play(void)
/* CPU �p���[��Q��Ȃ��悤�ɂ��邽�߂ɁC�ꎞ�I�ɒ�~���܂��D*/
{
    mmeWaitForCallbacks();
}
