#include <stdbool.h>

#include "magic.h"
#include "board.h"
#include "print.h"

typedef struct MagicNum{
	u64 magic;
	u64 occupancyMask;
}MagicNum;

MagicNum rookMagics[64];
MagicNum bishopMagics[64];

static bool testMagic(u64 magic, u64 occupancyMask){
	return false;
}

static u64 generateBishopMask(int square){
	u64 mask = 0;
	int x = square%8;
	int y = square/8;
	return (FILE_A<<x) | (RANK_1<<(y*8));
}

static u64 generateRookMask(int square){
	u64 mask = 0;
	int x = square%8;
	int y = square/8;
	mask = (FILE_A<<x) | (RANK_1<<(y*8));
	//reset the square the piece is on and the 4 squares on the edges of the board
	BBReset(mask, boardIndex(x,y));

	BBReset(mask, boardIndex(0,y));
	BBReset(mask, boardIndex(7,y));
	BBReset(mask, boardIndex(x,0));
	BBReset(mask, boardIndex(x,7));
	return mask;
}

void magicSearch(){
	for(int i = 0; i<64; i++){
		u64 rookMask = generateRookMask(i);
		u64 bishopMask = generateBishopMask(i);
		rookMagics[i].occupancyMask = rookMask;
		bishopMagics[i].occupancyMask = bishopMask;
		
		printBB(rookMask);
		//make random
		u64 magic = 0;
		testMagic(magic, rookMask);
	}	
}

void loadMagics(){
	magicSearch();
}
