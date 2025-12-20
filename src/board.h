#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t u64;
#define C64(x) (UINT64_C(x)) //used for defining u64 constants

//bit 0 in a bitboard represents a1, bit 1 b1, bit 2 c1, ect
//board indices are in the same order
/*
	8 9 ...
	0 1 2 3 4 5 6 7
*/
#define BOARD_INDEX(x,y) (((y)*8)+(x))

#define GET_BIT64(bb, index) ((bb)&((u64)1<<(index)))
#define SET_BIT64(bb, index) ((bb)|=((u64)1<<(index)))
#define RESET_BIT64(bb, index) ((bb)&=~((u64)1<<(index)))

#define RANK_1 C64(0x00000000000000FF)
#define RANK_2 C64(0x000000000000FF00)
#define RANK_3 C64(0x0000000000FF0000)
#define RANK_4 C64(0x00000000FF000000)
#define RANK_5 C64(0x000000FF00000000)
#define RANK_6 C64(0x0000FF0000000000)
#define RANK_7 C64(0x00FF000000000000)
#define RANK_8 C64(0xFF00000000000000)

#define FILE_A C64(0x0101010101010101)
#define FILE_B C64(0x0202020202020202)
#define FILE_C C64(0x0404040404040404)
#define FILE_D C64(0x0808080808080808)
#define FILE_E C64(0x1010101010101010)
#define FILE_F C64(0x2020202020202020)
#define FILE_G C64(0x4040404040404040)
#define FILE_H C64(0x8080808080808080)

#define MAX_FEN_SIZE 100 //assumes fullmove counter < 10^14
#define STARTPOS_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0"

//a black peice is a white peice +6
typedef enum Piece{
	P_NULL = -2,
	P_EMPTY = -1, //for squarewise board only
	P_PAWN,
	P_KNIGHT,
	P_BISHOP,
	P_ROOK,
	P_QUEEN,
	P_KING
}Piece;

#define P_BLACK 6
#define P_WHITE 0
#define P_IS_BLACK(piece) ((piece)>=6)
#define P_GET_COLOR(piece) (P_IS_BLACK(piece)? P_BLACK : P_WHITE)
#define P_GET_TYPE(piece) (P_IS_BLACK(piece) ? (piece) - 6 : (piece)) //convert any piece to a white piece

//Castling Rights
#define CR_WHITE_QUEENSIDE 0b00000001
#define CR_WHITE_KINGSIDE  0b00000010
#define CR_BLACK_QUEENSIDE 0b00000100
#define CR_BLACK_KINGSIDE  0b00001000

typedef struct Board{
	int squares[64];
	u64 bitboards[12];
	char castling_rights;
	bool whites_turn; 
	int ep_target; //target square for en passant, null if -1
	int halfmove_clock;
	int fullmoves;//fullmove counter, functions differently than the halfmove clock(doesnt reset)

	//stuff for move generation
	int color;//0 when turn = white, 6 when turn = black
	int enemy_color;
	u64 friendly_pieces;
	u64 enemy_pieces;
	u64 occupied;
}Board;


int bitscan_forward(u64 x);
u64 signed_shift(u64 x, int s);
int count_bits(u64 x);

void set_piece(Board* board, int square, int piece);
void update_perspective_variables(Board *board);
