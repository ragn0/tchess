#include "directions.h"
#include "tchess.h"
#include "generators.h"
#include <stdio.h>
#include <stdlib.h>

// -- GENERATOR MOVES --

// Pawn
static void gen_pawn(const Position *pos, Square sq, MoveList *list) {
	char promos[] = {'q', 'r', 'b', 'n'};
	Piece p = at(pos, sq);
	Color c = piece_color(p);
	int file = file_of(sq);
	int rank = rank_of(sq);
	// Forward moves 
	int r1 = rank + 1 * ((c == WHITE) ? 1 : -1);
	Square to = SQ(file, r1);
	if (in_board(file, r1) && at(pos, to) == NO_PIECE){
		// Normal move, check for promotion
		if (r1 == 7 || r1 == 0) {
			// Promotions
			char promos[] = {'q', 'r', 'b', 'n'};
			for (int i = 0; i < 4; i++) {
				Move m = {sq, to, PROMOTION, promos[i]};
				add_move(list, m);
			}
		} else {
			Move m = {sq, to, NORMAL, NO_PIECE};
			add_move(list, m);
			// Double move from starting rank
			if (rank == 1 || rank == 6) {
				int r2 = rank + 2 * ((c == WHITE) ? 1 : -1);
				Square to2 = SQ(file, r2);
				if (at(pos, to2) == NO_PIECE) {
					Move m2 = {sq, to2, NORMAL, NO_PIECE};
					add_move(list, m2);
				}
			}
		}
	}
	// Captures
	if (in_board(file - 1, r1) && in_board(file + 1, r1)) {
		Square cap_left = SQ(file - 1, r1);
		Square cap_right = SQ(file + 1, r1);

		Piece l_p = at(pos, cap_left);
		Piece r_p = at(pos, cap_right);

		if (l_p != NO_PIECE && piece_color(l_p) != c) {
			// Capture left, check for promotion
			if (r1 == 7 || r1 == 0) {
				for (int i = 0; i < 4; i++) {
					Move m = {sq, cap_left, PROMOTION, promos[i]};
					add_move(list, m);
				}
			} else {
				Move m = {sq, cap_left, NORMAL, NO_PIECE};
				add_move(list, m);
			}
		}
		if (r_p != NO_PIECE && piece_color(r_p) != c) {
			// Capture right, check for promotion
			if (r1 == 7 || r1 == 0) {
				for (int i = 0; i < 4; i++) {
					Move m = {sq, cap_left, PROMOTION, promos[i]};
					add_move(list, m);
				}
			} else {
				Move m = {sq, cap_left, NORMAL, NO_PIECE};
				add_move(list, m);
			}
		}
		
	}

	// En Passant
	if (pos->en_passant_target != NO_SQUARE) {
		int epf = file_of(pos->en_passant_target), epr = rank_of(pos->en_passant_target);
		if (c == WHITE){
			if ((epr == rank+1) && (epf == file - 1 || epf == file + 1)) {
				Move m = {sq, pos->en_passant_target, EN_PASSANT, NO_PIECE};
				add_move(list, m);
			}
		}
		else {
			if ((epr == rank-1) && (epf == file - 1 || epf == file + 1)) {
				Move m = {sq, pos->en_passant_target, EN_PASSANT, NO_PIECE};
				add_move(list, m);
			}
		}
	}
}

// Knight
static void gen_knight(const Position *pos, Square sq, MoveList *list) {
	Piece p = at(pos, sq);
	Color c = piece_color(p);
	int file = file_of(sq);
	int rank = rank_of(sq);
	for (int i = 0; i < 8; i++) {
		int df = KN[i][0];
		int dr= KN[i][1];
		if (!in_board(file + df, rank + dr)) continue;

		int nf = file + df;
		int nr = rank + dr;
		Square sq_to = SQ(nf, nr);
		Piece target = at(pos, sq_to);

		if (target == NO_PIECE) {
			Move m = (Move){sq, sq_to, NORMAL, NO_PIECE};
			add_move(list, m);
		}
		else if (target != NO_PIECE && piece_color(target) != c) {
			Move m = (Move){sq, sq_to, CAPTURE, NO_PIECE};
			add_move(list, m);
		}
	}
}

// Sliding pieces
static void gen_slider(const Position *pos, Square sq, MoveList *list, const int directions[][2], int num_dirs) {
	Piece p = at(pos, sq);
	Color c = piece_color(p);
	int file = file_of(sq);
	int rank = rank_of(sq);
	for (int i = 0; i < num_dirs; i++) {
		int df = directions[i][0];
		int dr = directions[i][1];
		int f = file + df;
		int r = rank + dr;
		while (in_board(f, r)) {
			Square sq_to = SQ(f, r);
			Piece target = at(pos, sq_to);
			if (target == NO_PIECE) {
				Move m = (Move){sq, sq_to, NORMAL, NO_PIECE};
				add_move(list, m);
			} else {
				if (piece_color(target) != c) {
					Move m = (Move){sq, sq_to, CAPTURE, NO_PIECE};
					add_move(list, m);
				}
				break; // Stop sliding on capture or blocked
			}
			f += df;
			r += dr;
		}
	
	}
}

// Normal King moves
static void gen_king(const Position *pos, Square sq, MoveList *list) {
	Piece p = at(pos, sq);
	Color c = piece_color(p);
	int file = file_of(sq);
	int rank = rank_of(sq);
	for (int df = -1; df <= 1; df++) 
		for (int dr = -1; dr <= 1; dr++) {
			if (df == 0 && dr == 0) continue;
			int f = file + df;
			int r = rank + dr;
			if (in_board(f, r)) {
				Square sq_to = SQ(f, r);
				Piece target = at(pos, sq_to);
				if (target == NO_PIECE) {
					Move m = (Move){sq, sq_to, NORMAL, NO_PIECE};
					add_move(list, m);
				}
				else if (piece_color(target) != c) {
					Move m = (Move){sq, sq_to, CAPTURE, NO_PIECE};
					add_move(list, m);
				}
			}
		}

}

// Castling moves
static void gen_castling(const Position *pos, MoveList *list) {
	if (pos->side_to_move == WHITE) {
		// Kingside
		if ((pos->castling_rights & WHITE_KING_SIDE_CASTLING) &&
			at(pos, F1) == NO_PIECE && at(pos, G1) == NO_PIECE) {
			Move m = {E1, G1, CASTLING_KINGSIDE, NO_PIECE};
			add_move(list, m);
		}
		// Queenside
		if ((pos->castling_rights & WHITE_QUEEN_SIDE_CASTLING) &&
			at(pos, D1) == NO_PIECE && at(pos, C1) == NO_PIECE && at(pos, B1) == NO_PIECE) {
			Move m = {E1, C1, CASTLING_QUEENSIDE, NO_PIECE};
			add_move(list, m);
		}
	} else {
		// Kingside
		if ((pos->castling_rights & BLACK_KING_SIDE_CASTLING) &&
			at(pos, F8) == NO_PIECE && at(pos, G8) == NO_PIECE) {
			Move m = {E8, G8, CASTLING_KINGSIDE, NO_PIECE};
			add_move(list, m);
		}
		// Queenside
		if ((pos->castling_rights & BLACK_QUEEN_SIDE_CASTLING) &&
			at(pos, D8) == NO_PIECE && at(pos, C8) == NO_PIECE && at(pos, B8) == NO_PIECE) {
			Move m = {E8, C8, CASTLING_QUEENSIDE, NO_PIECE};
			add_move(list, m);
		}
	}
}

// Based on the piece on the given square, we will call the appropriate generator
static void gen_piece_moves(const Position *pos, Square sq, MoveList *list) {
	Piece p = at(pos, sq);
	switch (piece_type(p)) {
		case PAWN:
			gen_pawn(pos, sq, list);
			break;
		case KNIGHT:
			gen_knight(pos, sq, list);
			break;
		case BISHOP:
			gen_slider(pos, sq, list, BDIR, 4);
			break;
		case ROOK:
			gen_slider(pos, sq, list, RDIR, 4);
			break;
		case QUEEN:
			gen_slider(pos, sq, list, QDIR, 8);
			break;
		case KING:
			gen_king(pos, sq, list);
			break;
		default:
			break;
	}	
}

// Generate all pseudo-legal moves for the side to move
void generate_pseudo_legal_moves(const Position *pos, MoveList *ml) {
	ml->count = 0;
	Color us = pos->side_to_move;
	for (Square sq = 0; sq < NUM_SQUARES; sq++) {
		Piece p = at(pos, sq);
		if (p != NO_PIECE && piece_color(p) == us) {
			gen_piece_moves(pos, sq, ml);
		}
	}
	gen_castling(pos, ml);
}

// Check if the squares the king passes through during castling are safe
static bool castle_path_safe(const Position* pos, Color side, bool kingside) {
    if (side == WHITE) {
        if (is_square_attacked(pos, E1, BLACK)) return false;
        if (kingside) {
            if (is_square_attacked(pos, F1, BLACK)) return false;
            if (is_square_attacked(pos, G1, BLACK)) return false;
        } else {
            if (is_square_attacked(pos, D1, BLACK)) return false;
            if (is_square_attacked(pos, C1, BLACK)) return false;
        }
    } else {
        if (is_square_attacked(pos, E8, WHITE)) return false;
        if (kingside) {
            if (is_square_attacked(pos, F8, WHITE)) return false;
            if (is_square_attacked(pos, G8, WHITE)) return false;
        } else {
            if (is_square_attacked(pos, D8, WHITE)) return false;
            if (is_square_attacked(pos, C8, WHITE)) return false;
        }
    }
    return true;
}

void generate_legal(const Position* pos, MoveList* ml) {
	MoveList *pseudo_legal = malloc(sizeof(MoveList));
	generate_pseudo_legal_moves(pos, pseudo_legal);
	ml->count = 0;

	Color us = pos->side_to_move;
	Square king_sq = (us == WHITE) ? E1 : E8;
	
	for (int i = 0; i < pseudo_legal->count; i++) {
		Move m = pseudo_legal->list[i];
		Position *new_pos = malloc(sizeof(Position));
		// Find king's square
		if (m.from == king_sq) {
			king_sq = m.to;
		} else {
		for (int sq = 0; sq < NUM_SQUARES; sq++) {
			Piece p = at(pos, sq);
			if (piece_type(p) == KING && piece_color(p) == us) {
				king_sq = sq;
				break;
			}
		}
	}
		*new_pos = *pos; // Copy current position
		printf("En passant target after move: %s\n", square_to_string(pos->en_passant_target));
		make_move(new_pos, &m); // Make the move	
		// Check if our king is in check in the new position
		if (!is_square_attacked(new_pos, king_sq, !us)) {
			// If the move is castling, ensure the path is safe
			if (m.type == CASTLING_KINGSIDE) {
				if (castle_path_safe(pos, us, true)) {
					add_move(ml, m);
				}
			} else if (m.type == CASTLING_QUEENSIDE) {
				if (castle_path_safe(pos, us, false)) {
					add_move(ml, m);
				}
			} else {
				add_move(ml, m);
			}
		}
		free(new_pos);
		
	}
}
