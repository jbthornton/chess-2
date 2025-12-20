#include "uci.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

char* trim_whitespace(char* str){
	//chop whitespace from beginning
	while(isspace(str[0]))
		str = &str[1];

	//chop whitespace from end
	while(isspace(str[strlen(str)-1]))
		str[strlen(str)-1] = 0;

	return str;
}

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

//all return a pointer to the first character after their part of the fen string(should be ' ' or 0)
static char* load_fen_position(Board* board, char* fen);
static char* load_fen_turn(Board* board, char* fen);
static char* load_fen_castling(Board* board, char* fen);
static char* load_fen_ep(Board* board, char* fen);
static char* load_fen_hmc(Board* board, char* fen);
static char* load_fen_fmc(Board* board, char* fen);

void load_fen(Board *board, char* fen){
	fen = trim_whitespace(fen);

	board->halfmove_clock = 0;
	board->fullmoves = 0;
	
	fen = load_fen_position(board, fen);
	if(fen[0] != ' ') error("load_fen(): expected ' ' after piece placement");

	fen = load_fen_turn(board, &fen[1]);
	if(fen[0] != ' ') error("load_fen(): expected ' ' after side to move");

	fen = load_fen_castling(board, &fen[1]);
	if(fen[0] != ' ') error("load_fen(): expected ' ' after castling rights");

	fen = load_fen_ep(board, &fen[1]);
	if(fen[0] != ' ' && fen[0] != 0) //strings dont have to include hmc/fmc
		error("load_fen(): expected ' ' or string termination after en passant square");


	update_perspective_variables(board);

	if(fen[0] == 0) return; //accept strings that dont include half/full move counters
	fen = load_fen_hmc(board, &fen[1]);
	if(fen[0] == 0) error("load_fen(): must include both the halfmove clock and the fullmove counter, or exclude both");
	if(fen[0] != ' ') error("load_fen(): expected ' ' after halfmove clock");

	fen = load_fen_fmc(board, &fen[1]);
}

//in load_fen_* functions unexpected character also catches the string being too short

static char* load_fen_position(Board* board, char* fen){
	for(int i = 0; i<12; i++){
		board->bitboards[i] = (u64)0;
	}
	for(int i = 0; i<64; i++) board->squares[i] = P_EMPTY;

	int i = 0;
	int len = strlen(fen);
	int x = 0;
	int y = 7;
	while(!(y == 0 && x > 7)){
		if(i>=len)
			error("load_fen_position(): fen cut short :(");

		char c = fen[i++];

		if(c == '/'){
			if(x!=8) error("load_fen_position(): missing squares");
			y--;
			x = 0;
			continue;
		}

		if(x>=8) error("load_fen_position(): missing '/' character");

		if(isalpha(c)){
			int piece = char_to_piece(c);
			set_piece(board, BOARD_INDEX(x, y), piece);
			x++;
			continue;
		}

		if(isdigit(c)){
			x += c - '0';
			continue;
		}

		error("load_fen_position(): unexpected character");
	}
	return &fen[i];
}

static char* load_fen_turn(Board* board, char* fen){
	switch(fen[0]){
		case 'w':
			board->whites_turn = true;
			break;

		case 'b':
			board->whites_turn = false;
			break;

		default:
			error("load_fen(): expected 'w' | 'b' after peice placement");
	}
	return &fen[1];
}

static char* load_fen_castling(Board* board, char* fen){
	board->castling_rights = 0;
	if(fen[0] == '-')
		return &fen[1];

	for(int i = 0; i<4; i++){
		if(fen[0] == ' '){
			if(i == 0) error("load_fen_castling(): no castling section");
			break;
		}

		switch(fen[0]){
			case 'K':
				board->castling_rights |= CR_WHITE_KINGSIDE;
				break;
			case 'k':
				board->castling_rights |= CR_BLACK_KINGSIDE;
				break;
			case 'Q':
				board->castling_rights |= CR_WHITE_QUEENSIDE;
				break;
			case 'q':
				board->castling_rights |= CR_BLACK_QUEENSIDE;
				break;
			default:
				error("load_fen_castling(): unexpected character");
		}

		fen = &fen[1];
	}
	return fen;
}

static char* load_fen_ep(Board* board, char* fen){
	if(fen[0] == '-'){
		board->ep_target = -1;
		return &fen[1];
	}

	if(!is_square(fen))//is_square only looks at the 1st 2 characters in the str argument
		error("load_fen_ep(): expected square or '-'");

	board->ep_target = str_to_square(fen);
	return &fen[2];
}

static char* load_fen_hmc(Board* board, char* fen){
	if(!isdigit(fen[0]))
		error("load_fen_hmc(): expected digit");
	char* end;
	board->halfmove_clock = strtol(fen, &end, 10);
	return end;
}

static char* load_fen_fmc(Board* board, char* fen){
	if(!isdigit(fen[0]))
		error("load_fen_fmc(): expected digit");
	char* end;
	board->fullmoves = strtol(fen, &end, 10);
	return end;
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
