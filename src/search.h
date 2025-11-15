#pragma once
#include "board.h"
#include "move.h"
#include "movegen.h"

float perft(char* fen, int depth, int expected, bool divided);//returns time of search in seconds
