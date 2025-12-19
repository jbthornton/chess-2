#include <time.h>
#include <stdio.h>

#include "search.h"
#include "eval.h"
#include "uci.h"

static int nmax(Board *board, int depth);
Move search(Board board, int depth){
	Move bestMove;
	int bestEval = 0;
	bool moveFound = false;
	MoveArray legalMoves;
	generateMoves(&board, &legalMoves);
	for(int i = 0; i<legalMoves.length; i++){
		Unmove unmove = make_move(&board, legalMoves.moves[i]);
		int kingSquare = bitscan_forward(board.bitboards[P_KING+board.enemyColor]);//colors swapped after make_move
		if(squareThreatenedBy(&board, kingSquare, board.color)){
			unmake_move(&board, unmove);
			continue;
		}
		int eval = nmax(&board, depth-1);
		unmake_move(&board, unmove);
		if(eval>bestEval || !moveFound){
			bestEval = eval;
			bestMove = legalMoves.moves[i];
		}
		moveFound = true;
	}
	return bestMove;
}


static int nmax(Board *board, int depth){
	if(depth <= 0) return -evaluate(board);
	int bestEval = 0;
	bool moveFound = false;
	MoveArray legalMoves;
	generateMoves(board, &legalMoves);
	for(int i = 0; i<legalMoves.length; i++){
		Unmove unmove = make_move(board, legalMoves.moves[i]);
		int kingSquare = bitscan_forward(board->bitboards[P_KING+board->enemyColor]);//colors swapped after make_move
		if(squareThreatenedBy(board, kingSquare, board->color)){
			unmake_move(board, unmove);
			continue;
		}
		int eval = nmax(board, depth-1);
		unmake_move(board, unmove);
	
		if(eval>bestEval || !moveFound){
			bestEval = eval;
		}
		moveFound = true;
	}
	//return -eval because nmax() is evaluating how good a move is for the other player
	if(!moveFound)//checkmate
		return -LOSS_EVAL;
	return -bestEval;
}

static int perftSearch(Board* board, int depth);
float perft(char* fen, int depth, int expected, bool divided){
	Board board;
	load_fen(&board, fen);
	int result = 0;
	
	clock_t beginning = clock();
	MoveArray legalMoves;
	legalMoves.length = 0;
	generateMoves(&board, &legalMoves);

	for(int i = 0; i<legalMoves.length; i++){
		Unmove unmove = make_move(&board, legalMoves.moves[i]);
		int kingSquare = bitscan_forward(board.bitboards[P_KING+board.enemyColor]);//colors swapped after make_move
		if(!squareThreatenedBy(&board, kingSquare, board.color)){ //skip moves that put "us" in check(illegal)
			int nodeCount = perftSearch(&board, depth-1);
			result += nodeCount;
			if(divided){
				print_move(legalMoves.moves[i]);
				printf(": %d\n", nodeCount);
			}
		}
		unmake_move(&board, unmove);
	
	}
	clock_t end = clock();
	clock_t elapsed = end-beginning;
	float elapsedSeconds = (float)elapsed/CLOCKS_PER_SEC;
	if(expected != 0) printf("Depth: %d, %d/%d, Error: %d, Time: %fs\n", depth, result, expected, result-expected, elapsedSeconds);
	if(expected == 0) printf("Depth: %d, %d, Time: %fs\n", depth, result, elapsedSeconds);
	return elapsedSeconds;
}

static int perftSearch(Board* board, int depth){
	if(depth == 0) return 1;

	MoveArray legalMoves;
	legalMoves.length = 0;
	generateMoves(board, &legalMoves);

	int nodeCount = 0;
	for(int i = 0; i<legalMoves.length; i++){
		Unmove unmove = make_move(board, legalMoves.moves[i]);
		int kingSquare = bitscan_forward(board->bitboards[P_KING+board->enemyColor]);//colors swapped after make_move
		if(!squareThreatenedBy(board, kingSquare, board->color)) //skip moves that put "us" in check(illegal)
			nodeCount += perftSearch(board, depth-1);
		unmake_move(board, unmove);
	}
	return nodeCount;
}
