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

#include "timidity.h"
#include "aenc.h"
#include "audriv.h"
#include "timer.h"

#ifdef DEBUG
#include "audio_cnv.h"
#include <math.h>
#endif

void (* audriv_error_handler)(const char *errmsg) = NULL;

static int play_sample_rate = 8000;
static int play_sample_size = 1;
static double play_start_time;
static long play_counter, reset_samples;
static int play_encoding   = AENC_G711_ULAW;
static int play_channels = 1;
static int output_port = AUDRIV_OUTPUT_SPEAKER;
static int play_volume   = 0;
static Bool play_open_flag = False;

char audriv_errmsg[BUFSIZ];

static void audriv_err(const char *msg)
{
    strncpy(audriv_errmsg, msg, sizeof(audriv_errmsg) - 1);
    if(audriv_error_handler != NULL)
	audriv_error_handler(audriv_errmsg);
}

Bool audriv_setup_audio(void)
/* �I�[�f�B�I�̏��������s���܂��D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    return True;
}

void audriv_free_audio(void)
/* audio �̌㏈�����s���܂��D
 */
{
}

Bool audriv_play_open(void)
/* audio �����t�p�ɊJ���C���ł� audriv_write() �ɂ�艉�t�\��
 * ��Ԃɂ��܂��D���ɊJ���Ă���ꍇ�͂Ȃɂ��s���܂���D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    if(play_open_flag)
	return True;
    play_counter = 0;
    reset_samples = 0;
    play_open_flag = True;
    return True;
}

void audriv_play_close(void)
/* ���t�p�ɃI�[�v�����ꂽ audio ����܂��D���łɕ��Ă���
 * �ꍇ�͂Ȃɂ��s���܂���D
 */
{
    play_open_flag = False;
}

static long calculate_play_samples(void)
{
    long n, ret;

    if(play_counter <= 0)
	return 0;

    n = (long)((double)play_sample_rate
	       * (double)play_sample_size
	       * (get_current_calender_time() - play_start_time));
    if(n > play_counter)
	ret = play_counter / play_sample_size;
    else
	ret = n / play_sample_size;

    return ret;
}

long audriv_play_stop(void)
/* ���t�𑦍��ɒ�~���C��~���O�̃T���v������Ԃ��܂��D
 * audriv_play_stop() �̌Ăяo���ɂ���āCaudio �͕��܂��D
 * audio �����ɕ��Ă���ꍇ�� audriv_play_stop() ���Ăяo�����ꍇ�� 0 ��
 * �Ԃ��܂��D
 * �G���[�̏ꍇ�� -1 ��Ԃ��܂��D
 */
{
    long n;

    n = audriv_play_samples();
    play_open_flag = False;
    return n;
}

Bool audriv_is_play_open(void)
/* audio �����t�ŃI�[�v������Ă���ꍇ�� True,
 * ���Ă���ꍇ�� False ��Ԃ��܂��D
 */
{
    return play_open_flag;
}

Bool audriv_set_play_volume(int volume)
/* ���t���ʂ� 0 ? 255 �͈͓̔��Őݒ肵�܂��D0 �͖����C255 �͍ő剹�ʁD
 * 0 ������ 0�C255 �𒴂���l�� 255 �ɓ����D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */
{
    if(volume < 0)
	play_volume = 0;
    else if(volume > 255)
	play_volume = 255;
    else
	play_volume = volume;
    return True;
}

int audriv_get_play_volume(void)
/* ���t���ʂ� 0 ? 255 ���œ��܂��D0 �͖����C255 �͍ő剹�ʁD
 * ���s����� -1 ��Ԃ��C�����łȂ��ꍇ�� 0 ? 255 ���̉��ʂ�Ԃ��܂��D
 */
{
    return play_volume;
}

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

#ifdef CFG_FOR_SF
static int record_volume = 0;
#endif
int audriv_get_record_volume(void)
/* �^�����ʂ� 0 ? 255 ���œ��܂��D0 �͖����C255 �͍ő剹�ʁD
 * ���s����� -1 ��Ԃ��C�����łȂ��ꍇ�� 0 ? 255 ���̉��ʂ�Ԃ��܂��D
 */
{
    return record_volume;
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
    long qsz;

    qsz = audriv_get_filled();
    if(qsz == -1)
	return -1;
    if(qsz == 0)
    {
	reset_samples += play_counter / play_sample_size;
	play_counter = 0; /* Reset start time */
    }
    if(play_counter == 0)
	play_start_time = get_current_calender_time();
    play_counter += n;
    return n;
}

Bool audriv_set_noblock_write(Bool noblock)
/* noblock �� True ���w�肷��ƁCaudriv_write() �Ăяo���Ńu���b�N���܂���D
 * False ���w�肷��ƁC�f�t�H���g�̏�Ԃɖ߂��܂��D
 * �����ɐ�������� True ���C���s����� False ��Ԃ��܂��D
 */
{
    return True;
}

int audriv_play_active(void)
/* ���t���Ȃ� 1�C���t���łȂ��Ȃ� 0, �G���[�Ȃ� -1 ��Ԃ��܂��D
 */
{
    long n;

    n = audriv_get_filled();
    if(n > 0)
	return 1;
    return n;
}

long audriv_play_samples(void)
/* ���݉��t���̃T���v���ʒu��Ԃ��܂��D
 */
{
    return reset_samples + calculate_play_samples();
}

long audriv_get_filled(void)
/* �I�[�f�B�I�o�b�t�@���̃o�C�g����Ԃ��܂��B
 * �G���[�̏ꍇ�� -1 ��Ԃ��܂��B
 */
{
    long n;

    if(play_counter <= 0)
	return 0;
    n = (int)((double)play_sample_rate
	      * play_sample_size
	      * (get_current_calender_time() - play_start_time));
    if(n > play_counter)
	return 0;
    return play_counter - n;
}

const long *audriv_available_encodings(int *n_ret)
/* �}�V�����T�|�[�g���Ă��邷�ׂĂ̕��������X�g��Ԃ��܂��Dn_ret �ɂ�
 * ���̎�ނ̐����C�Ԃ���܂��D������������킷�萔�l��
 * usffd.h ���ɒ�`����Ă��� enum usffd_data_encoding �̒l�ł��D
 * �Ԃ�l�� free ���Ă͂Ȃ�܂���D
 */
{
    static const long encoding_list[] =
    {
	AENC_SIGBYTE, AENC_UNSIGBYTE, AENC_G711_ULAW,
	AENC_G711_ALAW, AENC_SIGWORDB, AENC_UNSIGWORDB,
	AENC_SIGWORDL, AENC_UNSIGWORDL
    };

    *n_ret = 8;
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
	5512, 6615,
	8000, 9600, 11025, 16000, 18900, 22050, 32000, 37800, 44100, 48000
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
	    play_encoding = encoding;
	    play_sample_size = AENC_SAMPW(encoding) * play_channels;
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
	    play_sample_rate = sample_rate;
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
	    play_channels = channels;
	    return True;
	}
    return False;
}

void audriv_wait_play(void)
/* CPU �p���[��Q��Ȃ��悤�ɂ��邽�߂ɁC�ꎞ�I�ɒ�~���܂��D*/
{
}
