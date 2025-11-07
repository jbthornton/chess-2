#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "board.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "print.h"

#include "magic.h"

static bool isSquare(char* str){
	if(str[0]<'a' || str[0]>'h') return false;
	if(str[1]<'1' || str[1]>'8') return false;
	return true;
}

static int squareToIndex(char* str){
	return boardIndex(str[0]-'a',str[1]-'1');
}

static bool isMove(char* str){
	if(isSquare(str) && isSquare(&str[2])) return true;
	return false;
}

void cli(){
	char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	Board board;

	char input[100];
	loadFEN(&board, fen);
	
	bool quit = false;
	u64 highlighted = (u64)0;
	MoveArray legalMoves = moveArrayCreate();
	while(!quit){
		legalMoves.length = 0;//clear array but keep memory
		generateMoves(&board, &legalMoves);
		
		//print board
		if(board.whitesTurn) printf(" ---white goes---\n");//"blacks turn" and "black to go" are not centered
		else                 printf(" ---black goes---\n");
		printBoard(&board, highlighted);
		highlighted = (u64)0;
		
		printf(" :");
		fgets(input, sizeof(input), stdin);

		//remove whitespace characters from input	
		int start = -1;
		for(int i = 0; i<sizeof(input); i++){
			if(!isspace(input[i]) && start == -1)
				start = i;
			if((isspace(input[i]) || input[i] == 0) && start != -1){
				input[i] = 0;
				break;
			}
		}

		if(strcmp(&input[start], "exit") == 0){
			quit = true;
			continue;
		}

		if(strcmp(&input[start], "help") == 0){
			printf( " ---help---\n"
					" help - show help menu\n"
					" exit - exit program\n"
					" threat - highlight threatened squares\n"
					" index - print board indices\n"
					" <square><square><promotion> - make a move (eg b1c3)\n"
					" <square> - show legal moves from a square (eg b1)\n"
					" press enter to continue\n"
					);
			fgets(input, sizeof(input), stdin);
			continue;
		}

		if(strcmp(&input[start], "magic") == 0){
			magicSearch();
			continue;
		}

		if(strcmp(&input[start], "threat") == 0){
			for(int i = 0; i<64; i++){
				if(squareThreatenedBy(&board, i, board.enemyColor)) BBSet(highlighted, i);
			}
			continue;
		}

		if(strcmp(&input[start], "index") == 0){
			printIndices();
			continue;
		}

		if(strcmp(&input[start], "perft") == 0){
			perft();
			continue;
		}

		if(isMove(&input[start])){
			Move move;
			move.from = squareToIndex(&input[start]);
			move.to = squareToIndex(&input[start+2]);
			switch(tolower(input[start+4])){
				case 'n':
					move.promotion = P_KNIGHT;
					break;
				case 'r':
					move.promotion = P_ROOK;
					break;
				case 'b':
					move.promotion = P_BISHOP;
					break;
				case 'q':
					move.promotion = P_QUEEN;
					break;
				default:
					move.promotion = P_EMPTY;
					break;
			}
			printMove(move);
			makeMove(&board, move);
			BBSet(highlighted, move.to);
			BBSet(highlighted, move.from);
			continue;
		}
		
		if(isSquare(&input[start])){
			int from = squareToIndex(&input[start]);
			for(int i = 0; i<legalMoves.length; i++){
				if(legalMoves.moves[i].from == from){
					BBSet(highlighted, legalMoves.moves[i].to);
					printMove(legalMoves.moves[i]);
				}
			}
			continue;
		}

		printf("command unrecognised\n");
	}
	moveArrayDestroy(&legalMoves);
}

int main(int argc, char* argv[]){
	generateMoveTables();
	cli();
	return 0;
}
