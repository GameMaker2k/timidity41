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



static IS_RS_DATA_T is_resample_buffer_thread[CDM_JOB_NUM][AUDIO_BUFFER_SIZE + 8]; // + 8 oversampling


void compute_voice_scc_thread(int v, DATA_T *ptr, int32 count, int thread)
{
	compute_voice_scc_switch(v, count, ptr, is_resample_buffer_thread[thread]);
}


void compute_voice_mms_thread(int v, DATA_T *ptr, int32 count, int thread)
{
	compute_voice_mms_switch(v, count, ptr, is_resample_buffer_thread[thread]);
}

