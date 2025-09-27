#pragma once
#include "board.h"
#include "move.h"

void printBB(u64 bb);
void printBoard(Board *board, u64 highlighted);
void printBoardDebug(Board *board);
void printMove(Move m);
