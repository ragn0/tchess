#include "tchess.h"
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

// Auxiliary function to convert square index to string (e.g., 0 -> "a1")
char* square_to_string(int square) {
	char *buffer = malloc(3 * sizeof(char));
	if (square < 0 || square >= NUM_SQUARES) {
		strcpy(buffer, "??");
		return 0;
	}
	int file = square % 8;
	int rank = square / 8;
	buffer[0] = 'a' + file;
	buffer[1] = '1' + rank;
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

    // intestazione colonne
    printf("  ");
    if (!flipped) {
        for (int f = 0; f < 8; ++f) printf("  %c ", 'a' + f);
    } else {
        for (int f = 7; f >= 0; --f) printf("  %c ", 'a' + f);
    }
    printf("\n");

    // riga superiore di separatori
    printf("  +---+---+---+---+---+---+---+---+\n");

    for (int row = 0; row < 8; ++row) {
        // Etichetta riga a sinistra
        int rank_label = !flipped ? (8 - row) : (1 + row);
        printf("%d |", rank_label);

        for (int col = 0; col < 8; ++col) {
            // Mappatura coordinate di *display* -> modello
            // Assunzione: rank 0 in memoria = riga "8" (in alto), file 0 = 'a' (a sinistra).
            Piece p = pos->board[SQ(col, row)];
            char c  = piece_to_char(p);
            printf(" %c |", c);
        }

        // Etichetta riga a destra
        printf(" %d\n", rank_label);
        printf("  +---+---+---+---+---+---+---+---+\n");
    }

    // intestazione colonne in basso
    printf("  ");
    if (!flipped) {
        for (int f = 0; f < 8; ++f) printf("  %c ", 'a' + f);
    } else {
        for (int f = 7; f >= 0; --f) printf("  %c ", 'a' + f);
    }
    printf("\n\n");
}

// Rotate the board
void rotate_board(Position *pos) {
	for (int i = 0; i < 32; i++) {
		int temp = pos->board[i];
		pos->board[i] = pos->board[NUM_SQUARES-1 - i];
		pos->board[NUM_SQUARES-1 -i] = temp;
	}
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
	move->from = SQ(move_str[0] - 'a', move_str[1] - '1');
	move->to = SQ(move_str[2] - 'a', move_str[3] - '1');
	if (move->from < 0 || move->from >= NUM_SQUARES || 
		move->to < 0 || move->to >= NUM_SQUARES) {
		free(move);
		return NULL; // Invalid square
	}
	return move;
}

// Make a move on the board (doesn't check legality)
int make_move(Position *pos, const Move *move) {
    Square from = move->from;
    Square to   = move->to;
    Piece  moving = pos->board[from];
    if (moving == NO_PIECE) return 0;

    Color c_us = piece_color(moving);
    Color c_them = (c_us == WHITE ? BLACK : WHITE);

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

        // azzera halfmove clock: pedone ha mosso + c'è cattura
        pos->halfmove_clock = 0;
    }
    // 3) PROMOTION 
    else if (move->type == PROMOTION) {
        if (piece_type(moving) != PAWN) return 0;
        captured = pos->board[to]; // se c'è un pezzo è cattura regolare
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

        // Imposta EP target se pedone fa doppio passo
        if (piece_type(moving) == PAWN) {
            int dr = rank_of(to) - rank_of(from);
            if (dr == 2) {
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


