/*
 * Checkmate and additional rules:
 * 1. Check control; DONE
 * 2. Checkmate;
 * 3. Stalemate;
 * 4. Fifty-move rule;
 * 5. Threefold repetition;
 * 6. Insufficient material.
 */	

#ifndef RULES_H
#define RULES_H
#include "tchess.h"

typedef enum GameStatus { ONGOING, CHECKMATE, STALEMATE, DRAW_50, DRAW_REP, DRAW_INSUFF } GameStatus;

bool is_in_check(const Position* board, Color side);
GameStatus status(const Position* pos, Color side, int repetition_count);

#endif // RULES_H
