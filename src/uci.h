#include "board.h"
#include "move.h"

char piece_to_char(int p);
int char_to_piece(char p);

int str_to_square(char* str);
Move str_to_move(char* str, Board board);//board needed to infer move type

bool is_square(char* str);
bool is_move(char* str);


void load_FEN(Board *board, char* fen);

void print_square(int index);
void print_move(Move move);
void print_FEN(Board board);
