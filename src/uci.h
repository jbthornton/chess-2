#include "board.h"
#include "move.h"

char pieceToChar(int p);
int charToPiece(char p);
int strToSquare(char* str);

bool strIsSquare(char* str);
bool strIsMove(char* str);


void loadFEN(Board *board, char* fen);

void printSquare(int index);
void printMove(Move move);
void printFEN(Board board);
