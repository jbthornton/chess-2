#pragma once
#include "board.h"

void magicSearch();
void fillMagicTables();
void refineMagics(int iterations);

u64 getRookDestinations(int square, u64 occupancy);
u64 getBishopDestinations(int square, u64 occupancy);
