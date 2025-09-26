#include <stdio.h>

#include <stdlib.h>

#include "print.h"
#include "move.h"

void error(char* message){
	fprintf(stderr,"ERROR %s\n" ,message);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
	char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	Board board;
	loadFEN(&board, fen);
	printBoard(&board, (u64)0);
	
	Move m;
	m.from = boardIndex(3, 1);
	m.to = boardIndex(3, 3);
	makeMove(&board, m);
	printBoardDebug(&board);
	return 0;
}
