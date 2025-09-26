#pragma once
#include "board.h"

typedef struct Move{
	int to;
	int from;
}Move;

void makeMove(Board* board, Move move);
