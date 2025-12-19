#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "error.h"


static const int index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};


/**
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */

int bitscan_forward(u64 bb) {
	if(bb == 0) error("Bitscan forward requires input not to be 0\n");
	const u64 debruijn64 = (u64)(0x03f79d71b4cb0a89);
	/*
		multiply the isolated ls1b with a 64 bit debruijn sequence to get an index to the index64 table
		multiplying a number with a single bit set acts as a shift by the base 2 log of the number (0b1000*x is x<<4)
		because every substring of a debrujin sequence is unique, each possible ls1b will result in a unique 6 bit sequence
		in the last(most significant) 6 bits of the result, which can be used as an index in a lookup table
	*/
	return index64[((bb & -bb) * debruijn64) >> 58];
}

//positive is left shift, or up the board
u64 signed_shift(u64 x, int s){
	if(s>0) return x<<s;
	return x>>(-s);
}

int count_bits(u64 x){
	int count = 0;
	while(x){
		count++;
		x&=x-1;//reset ls1b
	}
	return count;
}

void update_perspective_variables(Board* board){
	board->color = board->whites_turn? P_WHITE : P_BLACK;
	board->enemy_color = board->whites_turn? P_BLACK : P_WHITE;
	board->occupied = 0;
	for(int i = 0; i<12; i++){
		board->occupied |= board->bitboards[i];
	}

	board->friendly_pieces = 0;
	for(int i = board->color; i<board->color+6; i++){
		board->friendly_pieces |= board->bitboards[i];
	}

	board->enemy_pieces = board->occupied & (~board->friendly_pieces);
}
