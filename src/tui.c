#include "tui.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "board.h"
#include "move.h"
#include "search.h"
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
	"    <square> - highlight legal moves\n"\
	"    <square><square> - make a move\n"\
	"    fen - print the current position in fen notation\n"\
	"    load <fen> - load a position from fen notation\n"\
	"replace <square> with the name of any square (eg:\"a1\")\n"

#define INPUT_BUFFER_SIZE 500

typedef struct TUIState{
	Board board;
	bool running;
	u64 highlighted;
	Move move_history[10];
}TUIState;

static void run_command(char* cmd, TUIState *state);
static void print_board(Board board, u64 h);
static void cmd_move(TUIState *state, Move move);
static void cmd_square(TUIState *state, int square);

void run_TUI(){
	struct TUIState state;
	load_fen(&state.board, STARTPOS_FEN);
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

	while(isspace(cmd[strlen(cmd)-1]))//ignore whitespace at end
		cmd[strlen(cmd)-1] = 0;

	if(strcmp(cmd, "exit") == 0){
		printf("bye\n");
		state->running = false;
		return;
	}

	if(strcmp(cmd, "help") == 0){
		printf(HELP);
		return;
	}

	if(strcmp(cmd, "fen") == 0){
		print_fen(state->board);
		return;
	}

	if(strncmp(cmd, "load ", 5) == 0 && strlen(cmd)>6){
		load_fen(&state->board, &cmd[5]);
		return;
	}

	if(is_move(cmd)){
		cmd_move(state, str_to_move(cmd, state->board));
		return;
	}

	if(is_square(cmd)){
		cmd_square(state, str_to_square(cmd));
		return;
	}

	printf("invalid command, try \"help\" for help\n");
}

static void cmd_move(TUIState *state, Move move){
	make_move(&state->board, move);
	printf("\n");
	print_board(state->board, ((u64)1 << move.from) | ((u64)1 << move.to));

	printf("thinking...\n");
	move = search(state->board, 4);
	make_move(&state->board, move);
	state->highlighted = ((u64)1 << move.from) | ((u64)1 << move.to);
}

static void cmd_square(TUIState *state, int square){
	MoveArray plegal_moves;
	gen_pseudolegal_moves(&state->board, &plegal_moves);
	for(int m = 0; m<plegal_moves.length; m++){
		if(plegal_moves.moves[m].from == square)
			state->highlighted |= (u64)1<<plegal_moves.moves[m].to;
	}
}
