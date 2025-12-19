#include "uci.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

char piece_to_char(int piece){
	if(piece<-1 || piece>P_KING+6) return '?';
	const char* pieceChars = " PNBRQKpnbrqk";
	return pieceChars[piece+1];
}

int str_to_square(char* str){
	return BOARD_INDEX(str[0]-'a',str[1]-'1');
}

Move str_to_move(char* str, Board board){
	Move move;
	move.from = str_to_square(str);
	move.to = str_to_square(&str[2]);
	move.type = M_NORMAL;

	int fromp = P_GET_TYPE(board.squares[move.from]);

	if(fromp == P_PAWN){
		if(strlen(str)>=4 && strchr("nrbq", str[4]))
			move.type = char_to_piece(toupper(str[4]));//promotion type
	}

	if(fromp == P_KING){
		if(move.to-move.from == -2 || move.to-move.from == 2)
			move.type = M_CASTLE;
	}

	return move;
}

int char_to_piece(char c){
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
			error("char_to_piece(), received invalid char");
	}
	if(islower(c))
		piece += 6;
	return piece;
}

bool is_square(char* str){
	if(strlen(str)<2) return false;
	if(str[0]<'a' || str[0]>'h') return false;
	if(str[1]<'1' || str[1]>'8') return false;
	return true;
}

bool is_move(char* str){
	if(strlen(str)<4) return false;
	if(is_square(str) && is_square(&str[2])) return true;
	return false;
}

void load_fen(Board *board, char* fen){
	int len = strlen(fen);
	int index = 0;
	for(int i = 0; i<12; i++){
		board->bitboards[i] = (u64)0;
	}
	for(int i = 0; i<64; i++) board->squares[i] = P_EMPTY;
	board->castling_rights = 0;
	board->halfmove_clock = 0;
	board->fullmoves = 0;
	board->ep_target = -1;
	
	for(int y = 7; y>=0; y--){
		int x = 0;
		while(x<8){
			if(isalpha(fen[index])){
				int piece = char_to_piece(fen[index]);
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
		if(fen[index] != '/' && y!=0) error("load_fen() missing '/'");
		index++;
	}
	if(index>=(len-1)) error("load_fen() fen is cut short or invalid");

	//turn
	if(fen[index] == 'w') board->whites_turn = true;
	else board->whites_turn = false;
	index++;
	
	index++;//skip ' '
	
	//castling rights	
	if(index>=(len-1)) error("load_fen() fen is cut short or invalid");
	if(fen[index] == '-'){
		index++;
	}else{
		for(int i = 0; i<4; i++){
			if(fen[index] == ' ') break;
			if(fen[index] == 'K') board->castling_rights |= CR_WHITE_KINGSIDE;
			if(fen[index] == 'Q') board->castling_rights |= CR_WHITE_QUEENSIDE;
			if(fen[index] == 'k') board->castling_rights |= CR_BLACK_KINGSIDE;
			if(fen[index] == 'q') board->castling_rights |= CR_BLACK_QUEENSIDE;
			index++;
		}
	}
	index++;//skip ' '
	if(index>=(len-1)) error("load_fen() fen is cut short or invalid");

	//en passan square
	if(fen[index] == '-'){
		index++;
	}else{
		board->ep_target = str_to_square(&fen[index]);
		index+=2;
	}
	if(index>=(len-1)) error("load_fen() fen is cut short or invalid");
	index++;//skip ' '

	update_perspective_variables(board);

	//half/full move counters
	if(index>=(len-1)) return; //accept strings that dont include half/full move counters
	board->halfmove_clock = atoi(&fen[index]);
	while(fen[index] != ' ') if(index++>=(len-1)) return;

	index++;//skip ' '

	if(index>=(len-1)) return;
	board->fullmoves = atoi(&fen[index]);
}

void print_square(int index){
	printf("%c%d", 'a' + (index%8), index/8);
}

void print_move(Move m){
	print_square(m.from);
	print_square(m.to);
	if(ispromotion(m))
		printf("%c", piece_to_char(m.type));
}

void print_fen(Board board){
	for(int y = 7; y>=0; y--){
		int emptyCount = 0;
		for(int x = 0; x<8; x++){
			int piece = board.squares[BOARD_INDEX(x,y)];
			if(piece == P_EMPTY) emptyCount++;
			if(piece != P_EMPTY){
				if(emptyCount != 0) printf("%d", emptyCount);
				emptyCount = 0;
				printf("%c", piece_to_char(piece));
			}
		}
		if(emptyCount != 0) printf("%d", emptyCount);
		printf("/");
	}

	printf(" %c ", (board.whites_turn)? 'w':'b');

	if(board.castling_rights&CR_WHITE_KINGSIDE) printf("K");
	if(board.castling_rights&CR_WHITE_QUEENSIDE) printf("Q");
	if(board.castling_rights&CR_BLACK_KINGSIDE) printf("k");
	if(board.castling_rights&CR_BLACK_QUEENSIDE) printf("q");
	if(board.castling_rights == 0) printf("-");

	if(board.ep_target == -1){
		printf(" - ");
	}else{
		print_square(board.ep_target);
	}

	printf(" %d %d", board.halfmove_clock, board.fullmoves);
}
