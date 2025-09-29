#include "tchess.h"
#include "directions.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
				pos->board[i*8 + j] = (i == 1) ? BLACK_PAWN : WHITE_PAWN;
			}
			else if (i == 0 || i == 7) {
				if (j == 0 || j == 7) {
					pos->board[i*8 + j] = (i == 0) ? BLACK_ROOK : WHITE_ROOK;
				} else if (j == 1 || j == 6) {
					pos->board[i*8 + j] = (i == 0) ? BLACK_KNIGHT : WHITE_KNIGHT;
				} else if (j == 2 || j == 5) {
					pos->board[i*8 + j] = (i == 0) ? BLACK_BISHOP : WHITE_BISHOP;
				} else if (j == 3) {
					pos->board[i*8 + j] = (i == 0) ? BLACK_QUEEN : WHITE_QUEEN;
				} else if (j == 4) {
					pos->board[i*8 + j] = (i == 0) ? BLACK_KING : WHITE_KING;
				}
			}
		}
	}
}

// Auxiliary function to convert square index to string (e.g., 0 -> "a1")
char* square_to_string(Square square) {
	char *buffer = malloc(3 * sizeof(char));
	if (square < 0 || square >= NUM_SQUARES) {
		strcpy(buffer, "??\0");
		return buffer;
	}
	int file = file_of(square);
	int rank = rank_of(square);
	buffer[0] = (char)('a' + file);
	buffer[1] = (char)('1' + rank);
	buffer[2] = '\0';
	return buffer;
}

// Auxiliary function to convert piece enum to character
char piece_to_char(Piece p) {
    switch (p) {
        case WHITE_PAWN:   return 'P';
        case WHITE_KNIGHT: return 'N';
        case WHITE_BISHOP: return 'B';
        case WHITE_ROOK:   return 'R';
        case WHITE_QUEEN:  return 'Q';
        case WHITE_KING:   return 'K';
        case BLACK_PAWN:   return 'p';
        case BLACK_KNIGHT: return 'n';
        case BLACK_BISHOP: return 'b';
        case BLACK_ROOK:   return 'r';
        case BLACK_QUEEN:  return 'q';
        case BLACK_KING:   return 'k';
        default:           return ' ';
    }
}

// Print current board position
void print_board(const Position *pos) {
    const int flipped = pos->side_to_move;

	// Column headers at the top
    printf("  ");
    if (!flipped) {
        for (int f = 0; f < 8; ++f) printf("  %c ", 'a' + f);
    } else {
        for (int f = 7; f >= 0; --f) printf("  %c ", 'a' + f);
    }
    printf("\n");

    printf("  +---+---+---+---+---+---+---+---+\n");
	if (flipped) {
    for (int row = 0; row < 8; row++) {
        int rank_label = (1 + row);
        printf("%d |", rank_label);

        for (int col = 7; col >= 0; col--) {
			// Assuming 0,0 is a8, 7,7 is h1
            Piece p = pos->board[SQ(col, row)];
            char c  = piece_to_char(p);
            printf(" %c |", c);
        }
        printf(" %d\n", rank_label);
		printf("  +---+---+---+---+---+---+---+---+\n");
    }
	}
	else {
		for (int row = 7; row >= 0; row--) {
			int rank_label = (1 + row);
			printf("%d |", rank_label);

			for (int col = 0; col < 8; col++) {
				// Assuming 0,0 is a8, 7,7 is h1
				Piece p = pos->board[SQ(col, row)];
				char c  = piece_to_char(p);
				printf(" %c |", c);
			}
			printf(" %d\n", rank_label);
			printf("  +---+---+---+---+---+---+---+---+\n");
			}
	}
	// Bottom column headers
    printf("  ");
    if (!flipped) {
        for (int f = 0; f < 8; ++f) printf("  %c ", 'a' + f);
    } else {
        for (int f = 7; f >= 0; --f) printf("  %c ", 'a' + f);
    }
    printf("\n\n");
}

// Parse the move string
Move* parse_move(const char *move_str) {
	Move *move = malloc(sizeof(Move));
	if (!move) {
		return NULL; // Memory allocation failed
	}
	if (strlen(move_str) < 4 || strlen(move_str) > 5) {
		free(move);
		return NULL; // Invalid move string length
	}
	if (strlen(move_str) == 5) {	
		move->promotionPiece = move_str[4];
	}
	else {
		move->promotionPiece = NO_PIECE;
	}
	move->from = SQ((int)(move_str[0] - 'a'), (int)(move_str[1] - '1'));
	move->to = SQ((int)(move_str[2] - 'a'), (int)(move_str[3] - '1'));
	if (move->from >= NUM_SQUARES || move->to >= NUM_SQUARES) {
		printf("%d %d\n", move->from, move->to);
		free(move);
		return NULL; // Invalid square
	}
	move->type = NORMAL; // Default move type
	return move;
}

// Make a move on the board (doesn't check legality)
int make_move(Position *pos, const Move *move) {
    Square from = move->from;
    Square to   = move->to;
    Piece  moving = pos->board[from];
    if (moving == NO_PIECE) return 0;

    Color c_us = pos->side_to_move;
    Color c_them = !c_us;
	if (piece_color(moving) != c_us) return 0;
    // default: no en passant target 
    pos->en_passant_target = NO_SQUARE;

    // halfmove clock: increments by default, reset to 0 if pawn move or capture 
    pos->halfmove_clock++;

    Piece captured = NO_PIECE;      // For tracking castling rights if a rook is captured 
    Square cap_sq = to;             // Square of captured piece, different from 'to' only in en passant 

    // --- SPECIAL MOVES ---

    // 1) CASTLING (moving king and rook) 
    if (move->type == CASTLING_KINGSIDE || move->type == CASTLING_QUEENSIDE) {
		// Verify it is the correct king
        if (!((c_us==WHITE && moving==WHITE_KING) || (c_us==BLACK && moving==BLACK_KING))) return 0;

        // Move king
        pos->board[to]   = moving;
        pos->board[from] = NO_PIECE;

        // Move rook 
        if (move->type == CASTLING_KINGSIDE) {
            Square rook_from = (c_us==WHITE) ? H1 : H8; // H1/H8
            Square rook_to   = (c_us==WHITE) ? F1 : F8; // F1/F8
            pos->board[rook_to]   = pos->board[rook_from];
            pos->board[rook_from] = NO_PIECE;
        } else {
            Square rook_from = (c_us==WHITE) ? A1 : A8; // A1/A8
            Square rook_to   = (c_us==WHITE) ? D1 : D8; // D1/D8
            pos->board[rook_to]   = pos->board[rook_from];
            pos->board[rook_from] = NO_PIECE;
        }

        // Remove castling rights for this side 
        if (c_us == WHITE)
            pos->castling_rights &= ~(WHITE_KING_SIDE_CASTLING | WHITE_QUEEN_SIDE_CASTLING);
        else
            pos->castling_rights &= ~(BLACK_KING_SIDE_CASTLING | BLACK_QUEEN_SIDE_CASTLING);
    }
    // 2) EN PASSANT 
    else if (move->type == EN_PASSANT) {
        if (piece_type(moving) != PAWN) return 0; // Must be a pawn 
        Square taken_sq = SQ(file_of(to), rank_of(to) - 1); // pedone catturato NON è su 'to'
        captured = pos->board[taken_sq];
        if (captured != WHITE_PAWN || captured != BLACK_PAWN) return 0; 

        pos->board[to]   = moving;
        pos->board[from] = NO_PIECE; 
        pos->board[taken_sq] = NO_PIECE;
        cap_sq = taken_sq;

        pos->halfmove_clock = 0;
    }
    // 3) PROMOTION 
    else if (move->type == PROMOTION) {
        if (piece_type(moving) != PAWN) return 0;
        captured = pos->board[to]; // if a piece is on 'to', it's a capture 
        pos->board[to]   = promo_to_piece(c_us, move->promotionPiece);
        pos->board[from] = NO_PIECE;
 
        pos->halfmove_clock = 0;
    }
    // 4) NORMAL MOVE/CAPTURE 
    else {
        captured = pos->board[to];

        pos->board[to]   = moving;
        pos->board[from] = NO_PIECE;

		// Halfmove clock reset if pawn move or capture
        if (piece_type(moving) == PAWN || captured != NO_PIECE)
            pos->halfmove_clock = 0;

		// Set en passant target if a pawn moved two squares
        if (piece_type(moving) == PAWN) {
            int dr = rank_of(to) - rank_of(from);
            if (dr == 2 || dr == -2) {
                int mid_r = (rank_of(from) + rank_of(to)) / 2;
                pos->en_passant_target = SQ(file_of(from), mid_r);
            }
        }
    }

    // --- UPDATE CASTLING RIGHTS ---

    // If king moves, lose both castling rights 
    if (moving == WHITE_KING)
        pos->castling_rights &= ~(WHITE_KING_SIDE_CASTLING | WHITE_QUEEN_SIDE_CASTLING);
    else if (moving == BLACK_KING)
        pos->castling_rights &= ~(BLACK_KING_SIDE_CASTLING | BLACK_QUEEN_SIDE_CASTLING);

    // If a rook moves from its original square, lose that castling right 
    if (moving == WHITE_ROOK) {
        if (from == A1) pos->castling_rights &= ~WHITE_QUEEN_SIDE_CASTLING;
        else if (from == H1) pos->castling_rights &= ~WHITE_KING_SIDE_CASTLING;
    } else if (moving == BLACK_ROOK) {
        if (from == A8) pos->castling_rights &= ~BLACK_QUEEN_SIDE_CASTLING;
        else if (from == H8) pos->castling_rights &= ~BLACK_KING_SIDE_CASTLING;
    }

	// If I capture a rook on its original square, opponent loses that castling right
    if (captured != NO_PIECE) {
        if (cap_sq == A1) pos->castling_rights &= ~WHITE_QUEEN_SIDE_CASTLING;
        else if (cap_sq == H1) pos->castling_rights &= ~WHITE_KING_SIDE_CASTLING;
        else if (cap_sq == A8) pos->castling_rights &= ~BLACK_QUEEN_SIDE_CASTLING;
        else if (cap_sq == H8) pos->castling_rights &= ~BLACK_KING_SIDE_CASTLING;
    }

    pos->side_to_move = c_them;

    if (c_them == WHITE) pos->fullmove_number++;

    return 1;
}

bool is_square_attacked(const Position *pos, Square sq, Color attacker) {
	// 1) Pawn attacks
	if (attacker == WHITE) {
		Square nw_p = sq + NW;
		Square ne_p = sq + NE;
		if (in_board_sq(nw_p) && pos->board[nw_p] == WHITE_PAWN) return true;
		if (in_board_sq(ne_p) && pos->board[ne_p] == WHITE_PAWN) return true;
	}
	else {
		Square sw_p = sq + SW;
		Square se_p = sq + SE;
		if (in_board_sq(sw_p) && pos->board[sw_p] == BLACK_PAWN) return true;
		if (in_board_sq(se_p) && pos->board[se_p] == BLACK_PAWN) return true;
	}
	// 2) Knight attacks
	for (int i = 0; i < 8; i++) {
		int k_f = file_of(sq) + KN[i][0];
		int k_r = rank_of(sq) + KN[i][1];
		Square k_sq = SQ(k_f, k_r);
		if (in_board_sq(k_sq)) {
			Piece p = pos->board[k_sq];
			if ((attacker == WHITE && p == WHITE_KNIGHT) ||
				(attacker == BLACK && p == BLACK_KNIGHT)) {
				return true;
			}
		}
	}
	// 3) Bishop/Queen/Rook attacks
	for (int dir = 0; dir < 8; dir++) {
		int new_f = file_of(sq);
		int new_r = rank_of(sq);
		while (true) {
			new_f += QDIR[dir][0];
			new_r += QDIR[dir][1];
			Square new_sq = SQ(new_f, new_r);
			if (!in_board_sq(new_sq)) break;
			Piece p = pos->board[new_sq];
			if (p != NO_PIECE) {
				if (piece_color(p) == attacker) {
					if (piece_type(p) == QUEEN) return true;
					if ((dir < 4) && piece_type(p) == ROOK) return true; // Rook directions
					if ((dir >= 4) && piece_type(p) == BISHOP) return true; // Bishop directions
				}
				break; // Blocked by any piece
			}
		}
	}
	// 4) King attacks
	for (int df = -1; df <= 1; df++) {
		for (int dr = -1; dr <= 1; dr++) {
			if (df == 0 && dr == 0) continue;
			int k_f = file_of(sq) + df;
			int k_r = rank_of(sq) + dr;
			Square k_sq = SQ(k_f, k_r);
			if (in_board_sq(k_sq)) {
				Piece p = pos->board[k_sq];
				if ((attacker == WHITE && p == WHITE_KING) ||
					(attacker == BLACK && p == BLACK_KING)) {
					return true;
				}
			}
		}
	}
	

	return false;
}

