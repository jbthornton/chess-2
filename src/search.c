#include <stdio.h>

#include "search.h"
#include "print.h"

static int perftSearch(Board board, Move move, int depth);
static void rperft(int depth, char* fen);

void perft(){
	char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	int depth = 5;
 	int expected = 4865609; 
	rperft(1, fen);
	rperft(2, fen);
	rperft(3, fen);
	rperft(4, fen);
	rperft(5, fen);
}

static void rperft(int depth, char* fen){
	Board board;
	loadFEN(&board, fen);
	Move m;
	m.to = 0;
	m.from = 0;
	int result = perftSearch(board, m, depth);
	printf("Depth: %d, Result: %d\n", depth, result);
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
