#pragma once

#include "board.h"
#include "move.h"

#define MA_SIZE 256
//218 is the theoretical maximum number of moves possible from a chess position

typedef struct MoveArray{
	Move moves[MA_SIZE];
	int length;//number of moves and 1st empty index
}MoveArray;


void moveArrayAppend(MoveArray *ma, Move move);

//has to be run before generateMoves
void fillMoveTables();

void generateMoves(Board* board, MoveArray *ma);
bool squareThreatenedBy(Board* board, int square, int enemyColor);
