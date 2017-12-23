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

    bitset.h

    Author: Masanao Izumo <mo@goice.co.jp>
    Create: Sun Mar 02 1997
*/

#ifndef ___BITSET_H_
#define ___BITSET_H_

typedef struct _Bitset
{
    int nbits;
    unsigned int *bits;
} Bitset;
#define BIT_CHUNK_SIZE ((unsigned int)(8 * sizeof(unsigned int)))

/*
 * Bitset �̏�����
 * ��������A�S�Ẵr�b�g�� 0 �ɏ����������
 */
extern void init_bitset(Bitset *bitset, int nbits);

/*
 * Bitset �̉��
 */
extern void finalize_bitset(Bitset *bitset);

/*
 * start �Ԗڂ̃r�b�g����Anbit ���A0 �ɃZ�b�g����B
 */
extern void clear_bitset(Bitset *bitset, int start_bit, int nbits);

/*
 * start �r�b�g����Anbits ���A����
 */
extern void get_bitset(const Bitset *bitset, unsigned int *bits_return,
		       int start_bit, int nbits);
/* get_bitset �� 1 �r�b�g�� */
extern int get_bitset1(Bitset *bitset, int n);

/*
 * start �r�b�g����Anbits ���Abits �ɃZ�b�g����
 */
extern void set_bitset(Bitset *bitset, const unsigned int *bits,
		       int start_bit, int nbits);
/* set_bitset �� 1 �r�b�g�� */
extern void set_bitset1(Bitset *bitset, int n, int bit);

/*
 * bitset �̒��� 1 �r�b�g���܂܂�Ă��Ȃ���� 0 ��Ԃ��C
 * 1 �r�b�g�ł��܂܂�Ă���ꍇ�� 0 �ȊO�̒l��Ԃ��D
 */
extern unsigned int has_bitset(const Bitset *bitset);

/* bitset �̒���\�� */
extern void print_bitset(Bitset *bitset);

#endif /* ___BITSET_H_ */
