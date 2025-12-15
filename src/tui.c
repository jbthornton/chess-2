#include "tui.h"

#include <stdio.h>
#include "board.h"
#include "move.h"
#include "uci.h"

#define C_DARK "\x1b[42m"
#define C_LIGHT "\x1b[47m"
#define C_TEXT "\x1b[30m"
#define C_CLEAR "\x1b[0m"

static void printBoard(Board board){
	for(int s = 0; s<64; s++){
		if((s+s/8)%2 == 0)
			printf(C_LIGHT);
		else
			printf(C_DARK);
		printf(C_TEXT"%c "C_CLEAR, pieceToChar(board.squares[s]));

		if(s%8 == 7) printf("\n"); 
	}
}

void runTUI(){
	Board board;
	loadFEN(&board, STARTPOS_FEN);
	printBoard(board);
	printf("hi\n");
}
