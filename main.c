#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint64_t u64;

//bits 0-3 are the piece type, if bit 4 is set it is black
//---cpppp

typedef enum Piece{
	P_EMPTY = -1, //for squarewise board only
	P_PAWN,
	P_KNIGHT,
	P_BISHOP,
	P_ROOK,
	P_QUEEN,
	P_KING
}Piece;

//peice masks
#define MP_BLACK 0b00010000
#define MP_PIECE 0b00001111 //ignore color

typedef struct Board{
	int squares[8][8];//x,y
	u64 whiteBBs[6];
	u64 blackBBs[6];
}Board;

//bit 0 in a bitboard represents a1, bit 1 b1, bit 2 c1, ect
/*
	8 9 ...
	0 1 2 3 4 5 6 7
*/

bool BBGet(u64 bb, int x, int y){
	return bb&((u64)1<<((y*8)+x));
}

void printBB(u64 bb){
	printf("  a b c d e f g h\n");//header
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if(BBGet(bb, x, y))
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black

			printf("  \x1b[0m");//"  " + color reset
		}
		printf("\n");
	}
}

void printBoard(Board *board, u64 highlighted){
	char* pieceChars = "PNBRQKpnbrqk";
	printf("  a b c d e f g h\n");//header
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if((x+y)%2 == 0)
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black

			if(BBGet(highlighted, x, y))
				printf("\x1b[44;30m");//background color blue, fg color black

			int piece = board->squares[x][y];
			if(piece == P_EMPTY){
				printf("  \x1b[0m");
				continue;
			}
			int index = piece&MP_PIECE;
			if(index>=0 && index<6){
				if(piece&MP_BLACK) index+=6;
				printf("%c ", pieceChars[index]);
			}
			else
				printf("!?");

			printf("\x1b[0m");//color reset
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]){
	
	Board board;
	for(int y = 0; y<8; y++){
		for(int x = 0; x<8; x++){
			board.squares[x][y] = (x%7)-1;
		}
	}
	printBoard(&board, 0);
	return 0;
}
