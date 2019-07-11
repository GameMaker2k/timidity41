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

#ifndef ___AUDRIV_H_
#define ___AUDRIV_H_

#ifndef Bool
#define Bool int
#endif

#ifndef False
#define False 0
#endif

#ifndef True
#define True 1
#endif

extern char audriv_errmsg[BUFSIZ];
/* �G���[�����������ꍇ�́C�G���[���b�Z�[�W���ݒ肳��܂��D
 * ���퓮�삵�Ă���ꍇ�́C�ύX����܂���D
 */

/* �I�[�f�B�I�̓��o�͐�������l */
enum audriv_ports
{
    AUDRIV_OUTPUT_SPEAKER,
    AUDRIV_OUTPUT_HEADPHONE,
    AUDRIV_OUTPUT_LINE_OUT
};


extern Bool audriv_setup_audio(void);
/* �I�[�f�B�I�̏��������s���܂��D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */

extern void audriv_free_audio(void);
/* audio �̌㏈�����s���܂��D
 */

extern Bool audriv_play_open(void);
/* audio �����t�p�ɊJ���C���ł� audriv_write() �ɂ�艉�t�\��
 * ��Ԃɂ��܂��D���ɊJ���Ă���ꍇ�͂Ȃɂ��s���܂���D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */

extern void audriv_play_close(void);
/* ���t�p�ɃI�[�v�����ꂽ audio ����܂��D���łɕ��Ă���
 * �ꍇ�͂Ȃɂ��s���܂���D
 */

extern long audriv_play_stop(void);
/* ���t�𑦍��ɒ�~���C��~���O�̃T���v������Ԃ��܂��D
 * audriv_play_stop() �̌Ăяo���ɂ���āCaudio �͕��܂��D
 * audio �����ɕ��Ă���ꍇ�� audriv_play_stop() ���Ăяo�����ꍇ�� 0 ��
 * �Ԃ��܂��D
 * �G���[�̏ꍇ�� -1 ��Ԃ��܂��D
 */

extern Bool audriv_is_play_open(void);
/* audio �����t�ŃI�[�v������Ă���ꍇ�� True,
 * ���Ă���ꍇ�� False ��Ԃ��܂��D
 */

extern Bool audriv_set_play_volume(int volume);
/* ���t���ʂ� 0 ? 255 �͈͓̔��Őݒ肵�܂��D0 �͖����C255 �͍ő剹�ʁD
 * 0 ������ 0�C255 �𒴂���l�� 255 �ɓ����D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */

extern int audriv_get_play_volume(void);
/* ���t���ʂ� 0 ? 255 ���œ��܂��D0 �͖����C255 �͍ő剹�ʁD
 * ���s����� -1 ��Ԃ��C�����łȂ��ꍇ�� 0 ? 255 ���̉��ʂ�Ԃ��܂��D
 */

extern Bool audriv_set_play_output(int port);
/* audio �̏o�͐� port ���w�肵�܂��Dport �ɂ͈ȉ��̂ǂꂩ���w�肵�܂��D
 *
 *     AUDRIV_OUTPUT_SPEAKER	�X�s�[�J�ɏo�́D
 *     AUDRIV_OUTPUT_HEADPHONE	�w�b�h�z���ɏo�́D
 *     AUDRIV_OUTPUT_LINE_OUT	���C���A�E�g�ɏo�́D
 *
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */

extern int audriv_get_play_output(void);
/* audio �̏o�͐� port �𓾂܂��D
 * ���s����� -1 ��Ԃ��C��������ƈȉ��̂����ꂩ�̒l��Ԃ��܂��D
 *
 *     AUDRIV_OUTPUT_SPEAKER	�X�s�[�J�ɏo�́D
 *     AUDRIV_OUTPUT_HEADPHONE	�w�b�h�z���ɏo�́D
 *     AUDRIV_OUTPUT_LINE_OUT	���C���A�E�g�ɏo�́D
 *
 */

extern int audriv_write(char *buff, int n);
/* audio �� buff �� n �o�C�g���������݂܂��D
 * audriv_set_noblock_write() �Ŕ�u���b�N�E���[�h���ݒ肳�ꂽ
 * �ꍇ�́C���̊֐��̌Ăяo���͑����ɏ������Ԃ�܂��D
 * �Ԃ�l�͎��ۂɗ������܂ꂽ�o�C�g���ł���C��u���b�N�E���[�h���ݒ�
 * ����Ă���ꍇ�́C���� n ��菭�Ȃ��ꍇ������܂��D
 * ���s����� -1 ��Ԃ��C��������ƁC���ۂɗ������܂ꂽ�o�C�g����Ԃ��܂��D
 */

extern Bool audriv_set_noblock_write(Bool noblock);
/* noblock �� True ���w�肷��ƁCaudriv_write() �Ăяo���Ńu���b�N���܂���D
 * False ���w�肷��ƁC�f�t�H���g�̏�Ԃɖ߂��܂��D
 * �����ɐ�������� True ���C���s����� False ��Ԃ��܂��D
 */

extern int audriv_play_active(void);
/* ���t���Ȃ� 1�C���t���łȂ��Ȃ� 0, �G���[�Ȃ� -1 ��Ԃ��܂��D
 */

extern long audriv_play_samples(void);
/* ���݉��t���̃T���v���ʒu��Ԃ��܂��D
 */

extern long audriv_get_filled(void);
/* �I�[�f�B�I�o�b�t�@���̃o�C�g����Ԃ��܂��B
 * �G���[�̏ꍇ�� -1 ��Ԃ��܂��B
 */

extern const long *audriv_available_encodings(int *n_ret);
/* �}�V�����T�|�[�g���Ă��邷�ׂĂ̕��������X�g��Ԃ��܂��Dn_ret �ɂ�
 * ���̎�ނ̐����C�Ԃ���܂��D������������킷�萔�l��
 * aenc.h ���ɒ�`����Ă���l�ł��D
 * �Ԃ�l�� free ���Ă͂Ȃ�܂���D
 */

extern const long *audriv_available_sample_rates(int *n_ret);
/* �}�V�����T�|�[�g���Ă��邷�ׂẴT���v�����[�g�̃��X�g��Ԃ��܂��D
 * �Ԃ�l�̃T���v�����[�g�͒Ⴂ���ɂȂ��ł��܂��D
 * n_ret �ɂ͂��̎�ނ̐����C�Ԃ���܂��D
 * �Ԃ�l�� free ���Ă͂Ȃ�܂���D
 */

extern const long *audriv_available_channels(int *n_ret);
/* �}�V�����T�|�[�g���Ă��邷�ׂẴ`���l�����̃��X�g��Ԃ��܂��D
 * n_ret �ɂ͂��̎�ނ̐����C�Ԃ���܂��D
 * �Ԃ�l�� free ���Ă͂Ȃ�܂���D
 */

extern Bool audriv_set_play_encoding(long encoding);
/* audio ���t���̕������������w�肵�܂��D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */

extern Bool audriv_set_play_sample_rate(long sample_rate);
/* audio ���t���̃T���v�����[�g���w�肵�܂��D
 * ���������ꍇ�� True ���C���s�����ꍇ�� False ��Ԃ��܂��D
 */

extern Bool audriv_set_play_channels(long channels);
/* ���t�p�̃`���l������ݒ肵�܂��D
 * ���s����� False ��Ԃ��C��������� True ��Ԃ��܂��D
 */

extern void (* audriv_error_handler)(const char *errmsg);
/* NULL �łȂ���΁C�G���[�����������ꍇ�Ăяo����܂��D
 */

extern void audriv_wait_play(void);
/* CPU �p���[��Q��Ȃ��悤�ɂ��邽�߂ɁC�ꎞ�I�ɒ�~���܂��D*/

#endif /* ___AUDRIV_H_ */
