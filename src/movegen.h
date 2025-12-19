#pragma once

#include "board.h"
#include "move.h"

#define MA_SIZE 256
//218 is the theoretical maximum number of moves possible from a chess position

typedef struct MoveArray{
	Move moves[MA_SIZE];
	int length;//number of moves and 1st empty index
}MoveArray;


void ma_append(MoveArray *ma, Move move);

//has to be run before generateMoves, fills lookup tables
void fill_move_tables();

void gen_pseudolegal_moves(Board* board, MoveArray *ma);
bool is_threatened(Board* board, int square, int enemyColor);
