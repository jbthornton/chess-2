#include <string.h>
#include <ctype.h>

#include "board.h"

bool BBGet(u64 bb, int x, int y){
	return bb&((u64)1<<((y*8)+x));
}

void loadFEN(Board *board, char* fen){
	int index = 0;
	int x = 0;
	int y = 7;
	while(index<strlen(fen)){
		if(isdigit(fen[index])){
			for(int i = 0; i<fen[index]-'0'; i++){
				board->squares[x][y] = P_EMPTY;
				x++;
			}
			index++;
		}
		if(fen[index] == '/'){
			y--;
			x = 0;
			index++;
		};
		if(isalpha(fen[index])){
			switch(tolower(fen[index])){
				case 'p':
					board->squares[x][y] = P_PAWN;
					break;
				case 'n':
					board->squares[x][y] = P_KNIGHT;
					break;
				case 'b':
					board->squares[x][y] = P_BISHOP;
					break;
				case 'r':
					board->squares[x][y] = P_ROOK;
					break;
				case 'k':
					board->squares[x][y] = P_KING;
					break;
				case 'q':
					board->squares[x][y] = P_QUEEN;
					break;
			}
			if(islower(fen[index])){
				board->squares[x][y] |= MP_BLACK;
			}
			x++;
			index++;
		}
	}	
}
