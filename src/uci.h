#include "board.h"
#include "move.h"

char* trim_whitespace(char* str);//removes whitespace at the end of the string and returns pointer to 1st solid character
char piece_to_char(int p);
int char_to_piece(char p);

int str_to_square(const char* str);
Move str_to_move(const char* str, Board board);//board needed to infer move type

bool is_square(const char* str);
bool is_move(const char* str);


void load_fen(Board *board, const char* fen);

void print_square(int index);
void print_move(Move move);
void print_fen(Board board);
