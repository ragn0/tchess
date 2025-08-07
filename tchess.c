#include "tchess.h"
#include <string.h>

// Initialize a new position
void init_position(Position *pos){
	memset(pos, 0, sizeof(Position));
	pos->side_to_move = WHITE;
	pos->castling_rights = WHITE_KING_SIDE_CASTLING |
						   WHITE_QUEEN_SIDE_CASTLING | 
						   BLACK_KING_SIDE_CASTLING | 
						   BLACK_QUEEN_SIDE_CASTLING;
	pos->en_passant_target = NO_SQUARE;
	pos->halfmove_clock = 0;
	pos->fullmove_number = 1;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			pos->board[i*8 + j] = NO_PIECE;
			if (i == 1 || i == 6) {
				pos->board[i*8 + j] = (i == 1) ? WHITE_PAWN : BLACK_PAWN;
			}
			else if (i == 0 || i == 7) {
				if (j == 0 || j == 7) {
					pos->board[i*8 + j] = (i == 0) ? WHITE_ROOK : BLACK_ROOK;
				} else if (j == 1 || j == 6) {
					pos->board[i*8 + j] = (i == 0) ? WHITE_KNIGHT : BLACK_KNIGHT;
				} else if (j == 2 || j == 5) {
					pos->board[i*8 + j] = (i == 0) ? WHITE_BISHOP : BLACK_BISHOP;
				} else if (j == 3) {
					pos->board[i*8 + j] = (i == 0) ? WHITE_QUEEN : BLACK_QUEEN;
				} else if (j == 4) {
					pos->board[i*8 + j] = (i == 0) ? WHITE_KING : BLACK_KING;
				}
			}
		}
	}
}


