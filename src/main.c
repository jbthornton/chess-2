#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "eval.h"

#include "tui.h"

int main(int argc, char* argv[]){
	fill_move_tables();
	run_TUI();
	return 0;
}
