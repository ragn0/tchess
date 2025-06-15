#include "tchess.h"
#include <stdio.h>

void init_chessboard(chessboard_t *cb){
	// Initialize the chessboard with default positions
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cb->board[i][j] = EMPTY_SQUARE;
		}
	}
	
	// Set pawns
	for (int j = 0; j < 8; j++) {
		cb->board[1][j] = WHITE_PAWN;
		cb->board[6][j] = BLACK_PAWN;
	}
	
	// Set rooks
	cb->board[0][0] = WHITE_ROOK; cb->board[0][7] = WHITE_ROOK;
	cb->board[7][0] = BLACK_ROOK; cb->board[7][7] = BLACK_ROOK;
	
	// Set knights
	cb->board[0][1] = WHITE_KNIGHT; cb->board[0][6] = WHITE_KNIGHT;
	cb->board[7][1] = BLACK_KNIGHT; cb->board[7][6] = BLACK_KNIGHT;
	
	// Set bishops
	cb->board[0][2] = WHITE_BISHOP; cb->board[0][5] = WHITE_BISHOP;
	cb->board[7][2] = BLACK_BISHOP; cb->board[7][5] = BLACK_BISHOP;
	
	// Set queens and kings
	cb->board[0][3] = WHITE_QUEEN; cb->board[0][4] = WHITE_KING;
	cb->board[7][3] = BLACK_QUEEN; cb->board[7][4] = BLACK_KING;
}


// Function to print the chessboard in a fancy way
void print_chessboard(chessboard_t *cb){
	printf("  a b c d e f g h\n");
	for (int i = 0; i < 8; i++) {
		printf("%d ", 8 - i);
		for (int j = 0; j < 8; j++) {
			switch (cb->board[i][j]) {
				case WHITE_PAWN: printf("P "); break;
				case BLACK_PAWN: printf("p "); break;
				case WHITE_ROOK: printf("R "); break;
				case BLACK_ROOK: printf("r "); break;
				case WHITE_KNIGHT: printf("N "); break;
				case BLACK_KNIGHT: printf("n "); break;
				case WHITE_BISHOP: printf("B "); break;
				case BLACK_BISHOP: printf("b "); break;
				case WHITE_QUEEN: printf("Q "); break;
				case BLACK_QUEEN: printf("q "); break;
				case WHITE_KING: printf("K "); break;
				case BLACK_KING: printf("k "); break;
default: printf(". ");
			}
		}
		printf("\n");
	}
	printf("\n");
}
