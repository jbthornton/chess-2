#include "tui.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "board.h"
#include "move.h"
#include "uci.h"

#define C_DARK "\x1b[42m"
#define C_LIGHT "\x1b[47m"
#define C_HIGHLIGHT "\x1b[44m"
#define C_TEXT "\x1b[30m"
#define C_CLEAR "\x1b[0m"

#define HELP\
	"valid commands:\n"\
	"    exit - exit the program\n"\
	"    help - show list of commands\n"\
	"    <square> - highlight a square\n"\
	"    <square><square> - make a move\n"\
	"    fen - print the current position\n"\
	"replace <square> with the name of any square (eg:\"a1\")\n"\

#define INPUT_BUFFER_SIZE 500

typedef struct TUIState{
	Board board;
	bool running;
	u64 highlighted;
	Move move_history[10];
}TUIState;

static void run_command(char* cmd, TUIState *state);
static void print_board(Board board, u64 h);

void run_TUI(){
	struct TUIState state;
	load_FEN(&state.board, STARTPOS_FEN);
	state.running = true;
	state.highlighted = 0;

	char* input = calloc(INPUT_BUFFER_SIZE, sizeof(char));
	while(state.running){
		print_board(state.board, state.highlighted);
		state.highlighted = 0;
		printf(":");
		fgets(input, INPUT_BUFFER_SIZE, stdin);
		run_command(input, &state);
		printf("\n");
	}
}

static void print_board(Board board, u64 h){
	if(board.whitesTurn) 
		printf("   white's turn\n");
	else
		printf("   black's turn\n");

	printf(" a b c d e f g h\n");
	for(int y = 7; y>=0; y--){
		printf("%d", y+1); 
		for(int x = 0; x<8; x++){
			if((x+y)%2 == 0)
				printf(C_LIGHT);
			else
				printf(C_DARK);

			if(GET_BIT64(h,BOARD_INDEX(x,y)))
				printf(C_HIGHLIGHT);

			printf(C_TEXT"%c "C_CLEAR, piece_to_char(board.squares[BOARD_INDEX(x,y)]));
		}	
		printf("%d\n", y+1); 
	}
	printf(" a b c d e f g h\n");
}

static void run_command(char* cmd, TUIState *state){
	while(isspace(cmd[0]))//ignore whitespace in beginning
		cmd = &cmd[1];
	size_t len = strlen(cmd);

	while(isspace(cmd[len-1]))//ignore whitespace at end
		len--;

	if(strncmp(cmd, "exit", len) == 0){
		printf("bye\n");
		state->running = false;
		return;
	}

	if(strncmp(cmd, "help", len) == 0){
		printf(HELP);
		return;
	}

	if(strncmp(cmd, "fen", len) == 0){
		print_FEN(state->board);
		return;
	}

	if(is_move(cmd)){
		makeMove(&state->board, str_to_move(cmd, state->board));
		return;
	}

	if(is_square(cmd)){
		state->highlighted = (u64)1<<str_to_square(cmd);
		return;
	}

	printf("invalid command, try \"help\" for help\n");
}
