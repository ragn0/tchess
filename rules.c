#include "rules.h"
#include "tchess.h"
#include "generators.h"
#include <stdlib.h>

// Check control
bool is_in_check(const Position* pos, Color side) {
	Square king_sq = find_king(pos, side);
	return is_square_attacked(pos, king_sq, side);
}

static Color bishop_square_color(Square sq) {
	int file = sq % 8;
	int rank = sq / 8;
	return (file + rank) % 2 == 0 ? WHITE : BLACK;
}

// Insufficient material detection
static bool insufficient_material(const Position* pos) {
	int white_material = 0;
	int black_material = 0;
	int white_bishops = 0;
	Color* white_bishop_colors = malloc(8 * sizeof(Color));
	int black_bishops = 0;
	Color* black_bishop_colors = malloc(8 * sizeof(Color));
	int white_knights = 0;
	int black_knights = 0;

	for (Square sq = A1; sq <= H8; sq++) {
		Piece piece = pos->board[sq];
		if (piece == NO_PIECE) continue;

		Color color = piece_color(piece);
		PieceType type = piece_type(piece);

		if (type == PAWN || type == ROOK || type == QUEEN) {
			return false; // Sufficient material
		}

		if (color == WHITE) {
			if (type == BISHOP){
				Color color = bishop_square_color(sq);
				white_bishop_colors[white_bishops] = color;
				white_bishops++;
			}
			else if (type == KNIGHT){
				Color color = bishop_square_color(sq);
				black_bishop_colors[black_bishops] = color;
				black_bishops++;
			}
			else if (type == KING) continue;
			white_material++;
		} else {
			if (type == BISHOP) black_bishops++;
			else if (type == KNIGHT) black_knights++;
			else if (type == KING) continue;
			black_material++;
		}
	}

	if (white_material < 2 && black_material == 0) {
		return true; // King vs King
	}
	if (white_material < 2 && black_material == 1 && black_knights == 1) {
		return true; // King vs King and Knight
	}
	if (black_material < 2 && white_material == 1 && white_knights == 1) {
		return true; 
	}
	if (white_material < 2 && black_material == 1 && black_bishops == 1) {
		return true; // King vs King and Bishop
	}
	if (black_material < 2 && white_material == 1 && white_bishops == 1) {
		return true; 
	}
	if (white_material < 2 && black_material == 2 && black_knights == 2) {
		return true; // King vs King and two knights
	}
	if (black_material < 2 && white_material == 2 && white_knights == 2) {
		return true; 
	}
	if (white_material <= 2 && black_bishops >= 2) {
		for (int i = 1; i < black_bishops; i++) {
			if (black_bishop_colors[i] != black_bishop_colors[0]) {
				return false;
			}
		}
		return true; // King vs King and two bishops on same color
	}

	return false; // Sufficient material

}

GameStatus status(const Position* pos, Color side, int repetition_count){
	MoveList* ml;
	generate_legal(pos, ml);

	if (ml->count == 0) {
		if (is_in_check(pos, side)) {
			return CHECKMATE; 
		} else {
			return STALEMATE; 
		}
	}
	else {
		if (repetition_count >= 3) {
			return DRAW_REP;
		}
		if (pos->halfmove_clock >= 50) {
			return DRAW_50;
		}
		if (insufficient_material(pos)){
			return DRAW_INSUFF;
		}
		return ONGOING;
	}

}

