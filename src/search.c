#include <stdio.h>

#include "search.h"
#include "print.h"

static int perftSearch(Board board, Move move, int depth);

void perft(char* fen, int depth, int expected){
	Board board;
	loadFEN(&board, fen);
	Move m;
	m.to = 0;
	m.from = 0;
	int result = perftSearch(board, m, depth);
	if(expected != 0) printf("Depth: %d, %d/%d, Error: %d\n", depth, result, expected, result-expected);
	if(expected == 0) printf("Depth: %d, %d\n", depth, result);
}

static int perftSearch(Board board, Move move, int depth){
	if(move.to != move.from)//temporary null move
		makeMove(&board, move);
	
	int enemyKing = P_KING+board.enemyColor;
	if(board.bitboards[enemyKing] == 0) printf("AAA\n");
	if(board.bitboards[P_KING+board.color] == 0) printf("AAA\n");
	if(squareThreatenedBy(&board, bitScanForward(board.bitboards[enemyKing]), board.color)) {
		return 0;
	}
	
	if(depth == 0) return 1;
	int nodeCount = 0;
	MoveArray legalMoves;
	legalMoves.length = 0;
	generateMoves(&board, &legalMoves);
	for(int i = 0; i<legalMoves.length; i++){
		nodeCount += perftSearch(board, legalMoves.moves[i], depth-1);
	}
	return nodeCount;
}
