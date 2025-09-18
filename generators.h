#ifndef GENERATORS_H
#define GENERATORS_H

#include "tchess.h"

// Move generation
// static void gen_pawn(const Position* pos, Square s, MoveList* ml);
// static void gen_knight(const Position* pos, Square s, MoveList* ml);
// static void gen_slider(const Position* pos, Square s, MoveList* ml, const int directions[][2], int num_directions);
// static void gen_king(const Position* pos, Square s, MoveList* ml);
// static void gen_castling(const Position* pos, MoveList* ml);
// static void gen_piece_moves(const Position* pos, Square s, MoveList* ml);

// Public move generation functions
void generate_pseudo_legal_moves(const Position* pos, MoveList* ml); // Generate all pseudo-legal moves for the current position
void generate_legal(const Position* pos, MoveList* ml); // Generate all legal moves for the current position
#endif // GENERATORS_H
