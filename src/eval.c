#include "eval.h"

static int count_material(Board* board);

int evaluate(Board* board){
	return count_material(board);
}

static int count_material(Board* board){
	int count = 0;
	count += count_bits(board->bitboards[P_PAWN+board->color])*100;
	count += count_bits(board->bitboards[P_KNIGHT+board->color])*300;
	count += count_bits(board->bitboards[P_BISHOP+board->color])*300;
	count += count_bits(board->bitboards[P_ROOK+board->color])*500;
	count += count_bits(board->bitboards[P_QUEEN+board->color])*900;

	count -= count_bits(board->bitboards[P_PAWN+board->enemy_color])*100;
	count -= count_bits(board->bitboards[P_KNIGHT+board->enemy_color])*300;
	count -= count_bits(board->bitboards[P_BISHOP+board->enemy_color])*300;
	count -= count_bits(board->bitboards[P_ROOK+board->enemy_color])*500;
	count -= count_bits(board->bitboards[P_QUEEN+board->enemy_color])*900;
	return count;
}
