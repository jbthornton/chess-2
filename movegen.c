#include <malloc.h>
#include "movegen.h"
#include "error.h"

MoveArray moveArrayCreate(){
	MoveArray ma;
	ma.length = 0;
	ma.size = 10;//revisit this later
	ma.moves = malloc(sizeof(Move)*ma.size);
	if(ma.moves == NULL) error("failed to allocate memory for moveArray\n");
	return ma;	
}

void moveArrayDestroy(MoveArray *ma){
	free(ma->moves);
	ma->moves = NULL;
}

void moveArrayAppend(MoveArray *ma, Move move){
	if(ma->length>=ma->size){
		ma->size*=1.5;
		ma->moves = realloc(ma->moves, ma->size);
		if(ma->moves == NULL) error("failed to allocate memory for moveArray\n");
	}
	ma->moves[ma->length++] = move;
}

// returns an array of all legal moves, numMoves is set to the length
MoveArray generateMoves(Board* board){
	MoveArray ma = moveArrayCreate();
	return ma;
}
