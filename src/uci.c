#include "uci.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

char pieceToChar(int piece){
	if(piece<-1 || piece>P_KING+6) return '?';
	const char* pieceChars = " PNBRQKpnbrqk";
	return pieceChars[piece+1];
}

int strToSquare(char* str){
	return BOARD_INDEX(str[0]-'a',str[1]-'1');
}

int charToPiece(char c){
	int piece = 0;
	switch(tolower(c)){
		case 'p':
			piece = P_PAWN;
			break;
		case 'n':
			piece = P_KNIGHT;
			break;
		case 'b':
			piece = P_BISHOP;
			break;
		case 'r':
			piece = P_ROOK;
			break;
		case 'k':
			piece = P_KING;
			break;
		case 'q':
			piece = P_QUEEN;
			break;
		default:
			error("charToPiece(), received invalid char");
	}
	if(islower(c))
		piece += 6;
	return piece;
}

bool strIsSquare(char* str){
	if(str[0]<'a' || str[0]>'h') return false;
	if(str[1]<'1' || str[1]>'8') return false;
	return true;
}

bool strIsMove(char* str){
	if(strIsSquare(str) && strIsSquare(&str[2])) return true;
	return false;
}

void loadFEN(Board *board, char* fen){
	int len = strlen(fen);
	int index = 0;
	for(int i = 0; i<12; i++){
		board->bitboards[i] = (u64)0;
	}
	for(int i = 0; i<64; i++) board->squares[i] = P_EMPTY;
	board->castlingRights = 0;
	board->halfmoveClock = 0;
	board->fullmoveClock = 0;
	board->enPassant = -1;
	
	for(int y = 7; y>=0; y--){
		int x = 0;
		while(x<8){
			if(isalpha(fen[index])){
				int piece = charToPiece(fen[index]);
				board->squares[BOARD_INDEX(x,y)] = piece;
				SET_BIT64(board->bitboards[piece], BOARD_INDEX(x,y));
				x++;
				index++;
			}
			if(isdigit(fen[index])){
				x += fen[index] - '0';
				index++;
			}
		}
		if(fen[index] != '/' && y!=0) error("loadFEN() missing '/'");
		index++;
	}
	if(index>=(len-1)) error("loadFEN() fen is cut short or invalid");

	//turn
	if(fen[index] == 'w') board->whitesTurn = true;
	else board->whitesTurn = false;
	index++;
	
	index++;//skip ' '
	
	//castling rights	
	if(index>=(len-1)) error("loadFEN() fen is cut short or invalid");
	if(fen[index] == '-'){
		index++;
	}else{
		for(int i = 0; i<4; i++){
			if(fen[index] == ' ') break;
			if(fen[index] == 'K') board->castlingRights |= CR_WHITE_KINGSIDE;
			if(fen[index] == 'Q') board->castlingRights |= CR_WHITE_QUEENSIDE;
			if(fen[index] == 'k') board->castlingRights |= CR_BLACK_KINGSIDE;
			if(fen[index] == 'q') board->castlingRights |= CR_BLACK_QUEENSIDE;
			index++;
		}
	}
	index++;//skip ' '

	//en passan square
	if(fen[index] == '-'){
		index++;
	}else{
		board->enPassant = strToSquare(&fen[index]);
		index+=2;
	}
	index++;//skip ' '

	updatePerspectiveVariables(board);

	//half/full move counters
	if(index>=(len-1)) return; //accept strings that dont include half/full move counters
	board->halfmoveClock = atoi(&fen[index]);
	while(fen[index] != ' ') if(index++>=(len-1)) return;

	index++;//skip ' '

	if(index>=(len-1)) return;
	board->fullmoveClock = atoi(&fen[index]);
}

void printSquare(int index){
	printf("%c%d", 'a' + (index%8), index/8);
}

void printMove(Move m){
	printSquare(m.from);
	printSquare(m.to);
	if(moveIsPromo(m))
		printf("%c", pieceToChar(m.type));
}

void printFEN(Board board){
	for(int y = 7; y>=0; y--){
		int emptyCount = 0;
		for(int x = 0; x<8; x++){
			int piece = board.squares[BOARD_INDEX(x,y)];
			if(piece == P_EMPTY) emptyCount++;
			if(piece != P_EMPTY){
				if(emptyCount != 0) printf("%d", emptyCount);
				emptyCount = 0;
				printf("%c", pieceToChar(piece));
			}
		}
		if(emptyCount != 0) printf("%d", emptyCount);
		printf("/");
	}

	printf(" %c ", (board.whitesTurn)? 'w':'b');

	if(board.castlingRights&CR_WHITE_KINGSIDE) printf("K");
	if(board.castlingRights&CR_WHITE_QUEENSIDE) printf("Q");
	if(board.castlingRights&CR_BLACK_KINGSIDE) printf("k");
	if(board.castlingRights&CR_BLACK_QUEENSIDE) printf("q");
	if(board.castlingRights == 0) printf("-");

	if(board.enPassant == -1){
		printf(" - ");
	}else{
		printSquare(board.enPassant);
	}

	printf(" %d %d", board.halfmoveClock, board.fullmoveClock);
}
