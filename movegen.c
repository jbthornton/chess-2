#include <malloc.h>
#include "movegen.h"
#include "error.h"
#include "print.h"
#include "magic.h"

static u64 knightDestinations[64];
static u64 kingDestinations[64];
static u64* rookDestinations[64];
static u64* bishopDestinations[64];

void generateMoveTables(){
	const int knightOffsets[16] = {
		 1, 2,
		 1,-2,
		-1, 2,
		-1,-2,
	
		 2, 1,
		 2,-1,
		-2, 1,
		-2,-1,
	};
	const int kingOffsets[16] = {
		 1,-1,
		 1, 0,
	 	 1, 1,

		 0,-1,
	 	 0, 1,

		-1,-1,
		-1, 0,
	 	-1, 1,	
	};
	//knight move tables
	for(int x = 0; x<8; x++){
		for(int y = 0; y<8; y++){
			int i = boardIndex(x,y);
			knightDestinations[i] = 0;
			for(int j = 0; j<16; j+=2){
				int dx = x +knightOffsets[j];
				int dy = y +knightOffsets[j+1];
				if(dx>=0 && dx<8 && dy>=0 && dy<8) BBSet(knightDestinations[i], boardIndex(dx, dy));
			}
			kingDestinations[i] = (u64)0;
			for(int j = 0; j<16; j+=2){
				int dx = x +kingOffsets[j];
				int dy = y +kingOffsets[j+1];
				if(dx>=0 && dx<8 && dy>=0 && dy<8) BBSet(kingDestinations[i], boardIndex(dx, dy));
			}
		}
	}

	fillMagicTables();
}

MoveArray moveArrayCreate(){
	MoveArray ma;
	ma.length = 0;
	ma.size = 10;//revisit this later
	ma.moves = malloc(sizeof(Move)*ma.size);
	if(ma.moves == NULL) error("failed to allocate memory for moveArray\n");
	return ma;	
}

void moveArrayDestroy(MoveArray* ma){
	free(ma->moves);
	ma->moves = NULL;
}

void moveArrayAppend(MoveArray* ma, Move move){
	if(ma->length>=ma->size){
		ma->size*=1.5;
		ma->moves = realloc(ma->moves, sizeof(Move)*ma->size);
		if(ma->moves == NULL) error("failed to allocate memory for moveArray\n");
	}
	ma->moves[ma->length++] = move;
}

bool isThreatened(Board* board, int square){
	int enemyColor = board->whitesTurn ? 6 : 0;
	u64 pawns  = board->bitboards[P_PAWN+enemyColor];
	//white pieces shift 9, 7
	//black shift -9 -7 
	//opposite shift because we care about the enemy
	u64 pawnDestinations = BBSignedShift(pawns & ~(FILE_A), (board->whitesTurn)? -9:7);
	pawnDestinations |= BBSignedShift(pawns & ~(FILE_H), (board->whitesTurn)? -7:9);
	if(BBGet(pawnDestinations, square)) return true;
	
	if(knightDestinations[square] & board->bitboards[P_KNIGHT+enemyColor]) return true;
	if(getBishopDestinations(square, board->occupancy) & board->bitboards[P_BISHOP+enemyColor]) return true;
	if(getRookDestinations(square, board->occupancy) & board->bitboards[P_ROOK+enemyColor]) return true;
	if(getBishopDestinations(square, board->occupancy) & board->bitboards[P_QUEEN+enemyColor]) return true;
	if(getRookDestinations(square, board->occupancy) & board->bitboards[P_QUEEN+enemyColor]) return true;
	if(kingDestinations[square] & board->bitboards[P_KING+enemyColor]) return true;
	return false;
}

static void genPawnMoves(Board* board, MoveArray* ma);
static void genKnightMoves(Board* board, MoveArray* ma);
static void genKingMoves(Board* board, MoveArray* ma);
static void genRookMoves(Board* board, MoveArray* ma);
static void genBishopMoves(Board* board, MoveArray* ma);
static void genQueenMoves(Board* board, MoveArray* ma);

static void genCastlingMoves(Board* board, MoveArray* ma);

void generateMoves(Board* board, MoveArray* ma){
	board->color = board->whitesTurn? 0 : 6;
	board->occupancy = 0;
	for(int i = 0; i<12; i++){
		board->occupancy |= board->bitboards[i];
	}

	board->friendlyPieces = 0;
	for(int i = board->color; i<board->color+6; i++){
		board->friendlyPieces |= board->bitboards[i];
	}
	board->enemyPieces = board->occupancy & (~board->friendlyPieces);
	
	genPawnMoves(board, ma);
	genKnightMoves(board, ma);
	genKingMoves(board, ma);
	genRookMoves(board, ma);
	genBishopMoves(board, ma);
	genQueenMoves(board, ma);
	genCastlingMoves(board, ma);
}

static void addMovesToDest(u64 destinations, int from, MoveArray* ma){
	while(destinations){
		int square = bitScanForward(destinations);
		destinations &= destinations-1;//reset ls1b
		Move m = (Move){.to = square, .from = from};
		moveArrayAppend(ma, m);
	}
}

//used inside genPawnMoves
static void addPawnMoves(u64 promoRank, u64 destinations, int shift, MoveArray* ma){
	u64 noPromotions = destinations & (~promoRank);
	u64 promotions = destinations & promoRank;
	
	//normal destinations
	while(noPromotions){
		int square = bitScanForward(noPromotions);
		noPromotions &= noPromotions-1;//reset ls1b
		Move m = (Move){.to = square, .from = square-shift, .promotion = P_EMPTY};
		moveArrayAppend(ma, m);
	}
	
	//promotions
	while(promotions){
		int square = bitScanForward(promotions);
		promotions &= promotions-1;//reset ls1b
		//use this const array to avoid depending on the numerical values of the PIECES enum
		const int promotionPieces[] = {P_QUEEN, P_KNIGHT, P_ROOK, P_BISHOP};
		for(int i = 0; i<4; i++){
			//.promotion is a peice type, dont add color
			Move m = (Move){.to = square, .from = square-shift, .promotion = promotionPieces[i]};
			moveArrayAppend(ma, m);
		}
	}
}

static void genPawnMoves(Board* board, MoveArray* ma){
	int shift    = (board->whitesTurn)? 8 : -8;
	u64 homeRank = (board->whitesTurn)? RANK_2 : RANK_7;
	u64 leftFile = (board->whitesTurn)? FILE_A : FILE_H;
	u64 rightFile = (board->whitesTurn)? FILE_H : FILE_A;
	u64 promoRank = (board->whitesTurn)? RANK_8 : RANK_1;

	//forward
	u64 destinations = BBSignedShift(board->bitboards[P_PAWN+board->color], shift)&(~board->occupancy);
	addPawnMoves(promoRank, destinations, shift, ma);
	
	//double forward
	destinations = BBSignedShift(board->bitboards[P_PAWN+board->color]&homeRank, shift*2)&(~board->occupancy);
	addPawnMoves(promoRank, destinations, shift*2, ma);
	
	u64 targets = board->enemyPieces;
	if(board->enPassant != -1) BBSet(targets, board->enPassant);
	//left captures 
	destinations = BBSignedShift(board->bitboards[P_PAWN+board->color]&(~leftFile), shift-1)&targets;
	addPawnMoves(promoRank, destinations, shift-1, ma);

	//right captures 
	destinations = BBSignedShift(board->bitboards[P_PAWN+board->color]&(~rightFile), shift+1)&targets;
	addPawnMoves(promoRank, destinations, shift+1, ma);
}

static void genKnightMoves(Board* board, MoveArray* ma){
	u64 friendlyKnights = board->bitboards[P_KNIGHT+board->color];
	
	while(friendlyKnights){
		int i = bitScanForward(friendlyKnights);
		friendlyKnights &= friendlyKnights-1;
		u64 destinations = knightDestinations[i] & ~board->friendlyPieces;
		addMovesToDest(destinations, i, ma);
	}
}

static void genKingMoves(Board* board, MoveArray* ma){
	int i = bitScanForward(board->bitboards[P_KING+board->color]);
	u64 destinations = kingDestinations[i] & ~board->friendlyPieces;
	addMovesToDest(destinations, i, ma);
}

static void genRookMoves(Board* board, MoveArray* ma){
	u64 friendlyRooks = board->bitboards[P_ROOK+board->color];

	while(friendlyRooks){
		int square = bitScanForward(friendlyRooks);
		friendlyRooks &= friendlyRooks-1;
		u64 destinations = getRookDestinations(square, board->occupancy);
		destinations &= ~board->friendlyPieces;
		addMovesToDest(destinations, square, ma);
	}
}

static void genBishopMoves(Board* board, MoveArray* ma){
	u64 friendlyBishops = board->bitboards[P_BISHOP+board->color];

	while(friendlyBishops){
		int square = bitScanForward(friendlyBishops);
		friendlyBishops &= friendlyBishops-1;
		u64 destinations = getBishopDestinations(square, board->occupancy);
		destinations &= ~board->friendlyPieces;
		addMovesToDest(destinations, square, ma);
	}
}

static void genQueenMoves(Board* board, MoveArray* ma){
	int square = bitScanForward(board->bitboards[P_QUEEN+board->color]);
	u64 destinations = getRookDestinations(square, board->occupancy);
	destinations |= getBishopDestinations(square, board->occupancy);
	destinations &= ~board->friendlyPieces;
	addMovesToDest(destinations, square, ma);
}

static void genCastlingMoves(Board* board, MoveArray* ma){
	//board.canCastle will be false if
	//  the king or relavant rook has moved
	//  the relevant rook has been captured
	//the kings safety, the safety of the squares bewteen the king and the rook, and the emptyness of those squares must be checked here
	int kingSquare = (board->whitesTurn)? 4 : 60;
	if(board->canCastle[ (board->whitesTurn)? 0 : 2 ]){//queenside castling rights
		int rookSquare = (board->whitesTurn)? 2 : 58;
		bool canCastle = true;
		for(int s = kingSquare; s>rookSquare; s--){
			if(board->squares[s] != P_EMPTY && board->squares[s] != P_KING+board->color){
				canCastle = false;
				break;
			}
			if(isThreatened(board, s)){
				canCastle = false;
				break;
			}
		}
		if(canCastle){
				Move move;
				move.from = kingSquare;
				move.to = rookSquare;
				moveArrayAppend(ma, move);
		}
	}

	if(board->canCastle[ (board->whitesTurn)? 0 : 2 ]){//kingside castling rights
		int rookSquare = (board->whitesTurn)? 6 : 62;
		bool canCastle = true;
		for(int s = kingSquare; s<rookSquare; s++){
			if(board->squares[s] != P_EMPTY && board->squares[s] != P_KING+board->color){
				canCastle = false;
				break;
			}
			if(isThreatened(board, s)){
				canCastle = false;
				break;
			}
		}
		if(canCastle){
				Move move;
				move.from = kingSquare;
				move.to = rookSquare;
				moveArrayAppend(ma, move);
		}
	}
}
