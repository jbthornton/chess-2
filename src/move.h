#pragma once
#include "board.h"


enum MOVE_TYPE{
	M_PROMO_KNIGHT = P_KNIGHT,
	M_PROMO_BISHOP = P_BISHOP,
	M_PROMO_ROOK = P_ROOK,
	M_PROMO_QUEEN = P_QUEEN,
	M_NORMAL,
	M_CASTLE,
};

typedef struct Move{
	int to;
	int from;
	enum MOVE_TYPE type;
}Move;

bool moveIsPromo(Move move);
void makeMove(Board* board, Move move);
