#pragma once
#include "board.h"

void magicSearch();
void loadMagics();

u64 getRookDesinations(int square, u64 occupancy);
u64 getBishopDesinations(int square, u64 occupancy);
