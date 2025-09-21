#include <stdio.h>
#include <stdbool.h>


typedef enum Piece{
	P_EMPTY = 0,
	P_PAWN,
	P_KNIGHT,
	P_BISHOP,
	P_ROOK,
	P_QUEEN,
	P_KING
}Piece;

typedef struct Board{
	Piece pieces[8][8];//x,y
	bool isWhite[8][8];
}Board;


void printBoard(Board *board){
	char* pieceChars = " PNBRQKpnbrqk";
	printf("  a b c d e f g h\n");//header
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if((x+y)%2 == 0)
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black

			printf(" ");
			Piece piece = board->pieces[x][y];
			if(!board->isWhite[x][y]&&piece!=P_EMPTY) piece+=6;
			printf("%c", pieceChars[piece]);

			printf("\x1b[0m");//color reset
		}
		printf("\n");
	}
}

int main(int argc, char* argv[]){
	Board board;
	for(int y = 0; y<8; y++){
		for(int x = 0; x<8; x++){
			board.pieces[x][y] = x%7;
			board.isWhite[x][y] = y%2==0; 
		}
	}
	printBoard(&board);
	return 0;
}
