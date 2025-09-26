#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "board.h"
#include "move.h"

#include "print.h"

bool isMove(char* str){
	// a-h 1-8 a-h 1-8 (eg d2d4)
	if(str[0]<'a' || str[0]>'h') return false;
	if(str[1]<'1' || str[1]>'8') return false;
	if(str[2]<'a' || str[2]>'h') return false;
	if(str[3]<'1' || str[3]>'8') return false;
	return true;
}

void cli(){
	char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	Board board;

	char input[100];
	loadFEN(&board, fen);
	bool quit = false;
	while(!quit){
		printBoard(&board, 0);
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
		if(isMove(&input[start])){
			Move move;
			move.from = boardIndex(input[start]-'a',input[start+1]-'1');
			move.to = boardIndex(input[start+2]-'a',input[start+3]-'1');
			makeMove(&board, move);
			continue;
		}		
	}
}

int main(int argc, char* argv[]){
	cli();
	return 0;
}
