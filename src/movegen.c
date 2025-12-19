#include "movegen.h"
#include "magic.h"
#include "error.h"
#include <stdio.h>

static u64 knightDestinations[64];
static u64 kingDestinations[64];

static void add_pawn_moves(Board* board, MoveArray* ma);
static void add_knight_moves(Board* board, MoveArray* ma);
static void add_king_moves(Board* board, MoveArray* ma);
static void add_rook_moves(Board* board, MoveArray* ma);
static void add_bishop_moves(Board* board, MoveArray* ma);
static void gen_queen_moves(Board* board, MoveArray* ma);
static void add_castling_moves(Board* board, MoveArray* ma);

//helper functions
static void add_from_shift(u64 promoRank, u64 destinations, int shift, MoveArray* ma);
static void add_moves_to_dest(u64 destinations, int from, MoveArray* ma);

void fill_move_tables(){
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
	
	for(int x = 0; x<8; x++){
		for(int y = 0; y<8; y++){
			int i = BOARD_INDEX(x,y);
			
			knightDestinations[i] = 0;
			for(int j = 0; j<16; j+=2){
				int dx = x +knightOffsets[j];
				int dy = y +knightOffsets[j+1];
				if(dx>=0 && dx<8 && dy>=0 && dy<8) SET_BIT64(knightDestinations[i], BOARD_INDEX(dx, dy));
			}

			kingDestinations[i] = (u64)0;
			for(int j = 0; j<16; j+=2){
				int dx = x +kingOffsets[j];
				int dy = y +kingOffsets[j+1];
				if(dx>=0 && dx<8 && dy>=0 && dy<8) SET_BIT64(kingDestinations[i], BOARD_INDEX(dx, dy));
			}
		}
	}

	//fill magic lookup tables
	fill_rook_table();
	fill_bishop_table();
}

void ma_append(MoveArray* ma, Move move){
	ma->moves[ma->length++] = move;
}

bool is_threatened(Board* board, int square, int enemy_color){
	u64 pawns = board->bitboards[P_PAWN+enemy_color];
	//white pieces shift 9, 7
	//black shift -9 -7 
	//opposite shift because we care about the enemy
	u64 pawnDestinations = signed_shift(pawns & ~(FILE_A), (enemy_color == 6)? -9:7);
	pawnDestinations |= signed_shift(pawns & ~(FILE_H), (enemy_color == 6)? -7:9);
	if(GET_BIT64(pawnDestinations, square)) return true;
	
	if(knightDestinations[square] & board->bitboards[P_KNIGHT+enemy_color]) return true;
	if(magic_bishop_lookup(square, board->occupied) & board->bitboards[P_BISHOP+enemy_color]) return true;
	if(magic_rook_lookup(square, board->occupied) & board->bitboards[P_ROOK+enemy_color]) return true;
	if(magic_bishop_lookup(square, board->occupied) & board->bitboards[P_QUEEN+enemy_color]) return true;
	if(magic_rook_lookup(square, board->occupied) & board->bitboards[P_QUEEN+enemy_color]) return true;
	if(kingDestinations[square] & board->bitboards[P_KING+enemy_color]) return true;
	return false;
}

void gen_pseudolegal_moves(Board* board, MoveArray* ma){
	ma->length = 0;
	add_pawn_moves(board, ma);
	add_knight_moves(board, ma);
	add_king_moves(board, ma);
	add_rook_moves(board, ma);
	add_bishop_moves(board, ma);
	gen_queen_moves(board, ma);
	add_castling_moves(board, ma);
}

static void add_moves_to_dest(u64 destinations, int from, MoveArray* ma){
	while(destinations){
		int square = bitscan_forward(destinations);
		destinations &= destinations-1;//reset ls1b
		Move m = (Move){.to = square, .from = from, .type = M_NORMAL};
		ma_append(ma, m);
	}
}

//used inside add_pawn_moves
static void add_from_shift(u64 promoRank, u64 destinations, int shift, MoveArray* ma){
	u64 noPromotions = destinations & (~promoRank);
	u64 promotions = destinations & promoRank;
	
	//normal destinations
	while(noPromotions){
		int square = bitscan_forward(noPromotions);
		noPromotions &= noPromotions-1;//reset ls1b
		Move m = (Move){.to = square, .from = square-shift, .type = M_NORMAL};
		ma_append(ma, m);
	}
	
	//promotions
	while(promotions){
		int square = bitscan_forward(promotions);
		promotions &= promotions-1;//reset ls1b

		const int promotions[] = {M_PROMO_QUEEN, M_PROMO_KNIGHT, M_PROMO_ROOK, M_PROMO_BISHOP};
		for(int i = 0; i<4; i++){
			//.promotion is a peice type, dont add color
			Move m = (Move){.to = square, .from = square-shift, .type = promotions[i]};
			ma_append(ma, m);
		}
	}
}

static void add_pawn_moves(Board* board, MoveArray* ma){
	int shift    = (board->whites_turn)? 8 : -8;
	u64 homeRank = (board->whites_turn)? RANK_2 : RANK_7;
	u64 promoRank = (board->whites_turn)? RANK_8 : RANK_1;

	//forward
	u64 destinations = signed_shift(board->bitboards[P_PAWN+board->color], shift)&(~board->occupied);
	add_from_shift(promoRank, destinations, shift, ma);
	
	//double forward
	u64 blockers = board->occupied;
	if(board->whites_turn){
		u64 rank3 = board->occupied&RANK_3;
		blockers |= rank3<<8;
	}else{
		u64 rank6 = board->occupied&RANK_6;
		blockers |= rank6>>8;
	}
	destinations = signed_shift(board->bitboards[P_PAWN+board->color]&homeRank, shift*2)&(~blockers);
	add_from_shift(promoRank, destinations, shift*2, ma);
	
	u64 targets = board->enemy_pieces;
	if(board->ep_target != -1) SET_BIT64(targets, board->ep_target);


	//captures 
	destinations = signed_shift(board->bitboards[P_PAWN+board->color]&(~FILE_H), shift+1)&targets;
	add_from_shift(promoRank, destinations, shift+1, ma);

	destinations = signed_shift(board->bitboards[P_PAWN+board->color]&(~FILE_A), shift-1)&targets;
	add_from_shift(promoRank, destinations, shift-1, ma);
}

static void add_knight_moves(Board* board, MoveArray* ma){
	u64 friendlyKnights = board->bitboards[P_KNIGHT+board->color];
	
	while(friendlyKnights){
		int i = bitscan_forward(friendlyKnights);
		friendlyKnights &= friendlyKnights-1;
		u64 destinations = knightDestinations[i] & ~board->friendly_pieces;
		add_moves_to_dest(destinations, i, ma);
	}
}

static void add_king_moves(Board* board, MoveArray* ma){
	if(board->bitboards[P_KING+board->color] == 0) {
		error("NO KING AAAAH!!!\n");
	}
	int i = bitscan_forward(board->bitboards[P_KING+board->color]);
	u64 destinations = kingDestinations[i] & ~board->friendly_pieces;
	add_moves_to_dest(destinations, i, ma);
}

static void add_rook_moves(Board* board, MoveArray* ma){
	u64 friendlyRooks = board->bitboards[P_ROOK+board->color];

	while(friendlyRooks){
		int square = bitscan_forward(friendlyRooks);
		friendlyRooks &= friendlyRooks-1;
		u64 destinations = magic_rook_lookup(square, board->occupied);
		destinations &= ~board->friendly_pieces;
		add_moves_to_dest(destinations, square, ma);
	}
}

static void add_bishop_moves(Board* board, MoveArray* ma){
	u64 friendlyBishops = board->bitboards[P_BISHOP+board->color];

	while(friendlyBishops){
		int square = bitscan_forward(friendlyBishops);
		friendlyBishops &= friendlyBishops-1;
		u64 destinations = magic_bishop_lookup(square, board->occupied);
		destinations &= ~board->friendly_pieces;
		add_moves_to_dest(destinations, square, ma);
	}
}

static void gen_queen_moves(Board* board, MoveArray* ma){
	u64 friendlyQueens = board->bitboards[P_QUEEN+board->color];
	while(friendlyQueens){
		int square = bitscan_forward(friendlyQueens);
		friendlyQueens &= friendlyQueens-1;
		u64 destinations = magic_rook_lookup(square, board->occupied);
		destinations |= magic_bishop_lookup(square, board->occupied);
		destinations &= ~board->friendly_pieces;
		add_moves_to_dest(destinations, square, ma);
	}
}

static void add_castling_moves(Board* board, MoveArray* ma){
	//board.canCastle will be false if
	//  the king or relavant rook has moved
	//  the relevant rook has been captured
	//the kings safety, the safety of the squares bewteen the king and the rook, and the emptyness of those squares must be checked here
	int kingSquare = (board->whites_turn)? 4 : 60;
	int kingside = (board->whites_turn)? CR_WHITE_KINGSIDE : CR_BLACK_KINGSIDE;
	int queenside = (board->whites_turn)? CR_WHITE_QUEENSIDE : CR_BLACK_QUEENSIDE;
	if(board->castling_rights&queenside){
		int rookSquare = (board->whites_turn)? 0 : 56;
		bool canCastle = true;
		for(int s = kingSquare-1; s>rookSquare; s--){
			if(board->squares[s] != P_EMPTY){
				canCastle = false;
				break;
			}
		}
		for(int s = kingSquare; s>=kingSquare-2; s--){
			if(is_threatened(board, s, board->enemy_color)){
				canCastle = false;
				break;
			}
		}
		if(canCastle){
				Move move;
				move.from = kingSquare;
				move.to = kingSquare-2;
				move.type = M_CASTLE;
				ma_append(ma, move);
		}
	}

	if(board->castling_rights&kingside){
		int rookSquare = (board->whites_turn)? 7 : 63;
		bool canCastle = true;
		for(int s = kingSquare+1; s<rookSquare; s++){
			if(board->squares[s] != P_EMPTY){
				canCastle = false;
				break;
			}

		}
		for(int s = kingSquare; s<=kingSquare+2; s++){
			if(is_threatened(board, s, board->enemy_color)){
				canCastle = false;
				break;
			}
		}
		if(canCastle){
				Move move;
				move.from = kingSquare;
				move.to = kingSquare+2;
				move.type = M_CASTLE;
				ma_append(ma, move);
		}
	}
}
