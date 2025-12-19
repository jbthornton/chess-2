#pragma once
#include "board.h"

void magic_search();
void fill_rook_table();
void fill_bishop_table();

u64 magic_rook_lookup(int square, u64 occupancy);
u64 magic_bishop_lookup(int square, u64 occupancy);
