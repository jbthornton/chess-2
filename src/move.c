#include "move.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static void movePiece(Board* board, int from, int to){
	int piece = board->squares[from];
	int captured = board->squares[to];

	//update square-centric board
	board->squares[to] = piece;
	board->squares[from] = P_EMPTY;

	//update bitboards
	RESET_BIT64(board->bitboards[piece], from);
	SET_BIT64(board->bitboards[piece], to);
	if(captured != P_EMPTY)
		RESET_BIT64(board->bitboards[captured], to);
}

bool moveIsPromo(Move move){
	return (move.type != M_NORMAL && move.type != M_CASTLE);
}

void makeMove(Board* board, Move move){
	int piece = board->squares[move.from];
	int captured = board->squares[move.to];
	int pieceType = piece-(board->color);
	int capturedType = captured-(board->enemyColor);
	movePiece(board, move.from, move.to);
	
	//Promotion
	if(moveIsPromo(move)){
		int promotion = move.type;//M_PROMO_QUEEN == P_QUEEN ect
		board->squares[move.to] = promotion+board->color;
		RESET_BIT64(board->bitboards[piece], move.to);
		SET_BIT64(board->bitboards[promotion+board->color], move.to);
	}
	
	//Castling
	if(move.type == M_CASTLE){
		//move the rook
		if(move.to == 62)//g8
			movePiece(board, 63, 61);//h8->f8

		if(move.to == 58)//c8
			movePiece(board, 56, 59);//a8->d8

		if(move.to == 6)//g1
			movePiece(board, 7, 5);//h1->f1

		if(move.to == 2)//c1
			movePiece(board, 0, 3);//a1->d1

		//because any castling move is a king move, castling rights will be correctly updated later 
	}

	//En Passant
	//correctly handle en passant capture
	if(pieceType == P_PAWN && move.to == board->enPassant){
		int capturedSquare = move.to+8;
		if(move.to>32)
			capturedSquare = move.to-8;
		captured = board->squares[capturedSquare];
		board->squares[capturedSquare] = P_EMPTY;
		RESET_BIT64(board->bitboards[captured], capturedSquare);
	}

	//set en passant target square if move was a double pawn push
	board->enPassant = -1;
	if(pieceType == P_PAWN){
		//set en passant if move was a double push
		if(move.to-move.from == 16)
			board->enPassant = move.to-8;
		if(move.to-move.from == -16)
			board->enPassant = move.to+8;
	}

	//Castling Rights
	if(pieceType == P_KING){
		if(board->whitesTurn)
			board->castlingRights &= ~(CR_WHITE_KINGSIDE|CR_WHITE_QUEENSIDE);
		else
			board->castlingRights &= ~(CR_BLACK_KINGSIDE|CR_BLACK_QUEENSIDE);
	}

	if(pieceType == P_ROOK){
		switch(move.from){
			case 7: //h1
				board->castlingRights &= ~CR_WHITE_KINGSIDE;
				break;
			case 0: //a1
				board->castlingRights &= ~CR_WHITE_QUEENSIDE;
				break;
			case 63: //h8
				board->castlingRights &= ~CR_BLACK_KINGSIDE;
				break;
			case 56: //a8
				board->castlingRights &= ~CR_BLACK_QUEENSIDE;
				break;
		}
	}

	if(capturedType == P_ROOK){
		switch(move.to){
			case 7: //h1
				board->castlingRights &= ~CR_WHITE_KINGSIDE;
				break;
			case 0: //a1
				board->castlingRights &= ~CR_WHITE_QUEENSIDE;
				break;
			case 63: //h8
				board->castlingRights &= ~CR_BLACK_KINGSIDE;
				break;
			case 56: //a8
				board->castlingRights &= ~CR_BLACK_QUEENSIDE;
				break;
		}
	}
	
	board->whitesTurn = !board->whitesTurn;
	updatePerspectiveVariables(board);
}
