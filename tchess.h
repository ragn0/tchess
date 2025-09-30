#ifndef TCHESS_H
#define TCHESS_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_FILES 8
#define NUM_RANKS 8
#define NUM_SQUARES (NUM_FILES * NUM_RANKS)

// TYPEDEFS
typedef int16_t Square;
#define SQ(file, rank) (Square)((7 - rank) * NUM_RANKS + file)

// Move types
typedef enum { NORMAL=0, CAPTURE, EN_PASSANT, CASTLING_KINGSIDE, CASTLING_QUEENSIDE, PROMOTION } MoveType;

// Castling rights bitmask
enum {
	WHITE_KING_SIDE_CASTLING = 0x01, // 0b0001
	WHITE_QUEEN_SIDE_CASTLING = 0x02, // 0b0010
	BLACK_KING_SIDE_CASTLING = 0x04, // 0b0100
	BLACK_QUEEN_SIDE_CASTLING = 0x08 // 0b1000
};

// Enumerate squares 
enum {
    A1 = SQ(0, 0), B1, C1, D1, E1, F1, G1, H1,
    A2 = SQ(0, 1), B2, C2, D2, E2, F2, G2, H2,
    A3 = SQ(0, 2), B3, C3, D3, E3, F3, G3, H3,
    A4 = SQ(0, 3), B4, C4, D4, E4, F4, G4, H4,
    A5 = SQ(0, 4), B5, C5, D5, E5, F5, G5, H5,
    A6 = SQ(0, 5), B6, C6, D6, E6, F6, G6, H6,
    A7 = SQ(0, 6), B7, C7, D7, E7, F7, G7, H7,
    A8 = SQ(0, 7), B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE = -1
};

typedef struct {
	Square from; // 0-63 for squares a1-h8
	Square to; // 0-63 for squares a1-h8
	MoveType type;
	char promotionPiece; // 'q', 'r', 'b', 'n' for promotion moves, '0' otherwise
} Move;

typedef enum {
	WHITE,
	BLACK,
	NO_COLOR
} Color;

typedef enum {
	NO_PIECE_TYPE = 0,
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
} PieceType;

typedef enum {
	NO_PIECE = 0,
	WHITE_PAWN,
	WHITE_KNIGHT,
	WHITE_BISHOP,
	WHITE_ROOK,
	WHITE_QUEEN,
	WHITE_KING,
	BLACK_PAWN,
	BLACK_KNIGHT,
	BLACK_BISHOP,
	BLACK_ROOK,
	BLACK_QUEEN,
	BLACK_KING,
} Piece;

// Chessboard as 1D array of pieces
typedef struct {
    Piece board[NUM_SQUARES];
    Color side_to_move;
	int8_t castling_rights; // bitmask: 0bKQkq (1 for available, 0 for unavailable)
    Square en_passant_target;   // NO_SQUARE if none
    int    halfmove_clock;
    int    fullmove_number;
} Position;

// Auxiliary functions
char piece_to_char(Piece piece);
char* square_to_string(Square square);

// Get color
static inline Color piece_color(Piece piece) {
	if (piece == NO_PIECE) {
		return NO_COLOR;
	}
	
	return (piece >= BLACK_PAWN) ? BLACK : WHITE;
}
// Get piece type
static inline PieceType piece_type(Piece piece) {
	if (piece == NO_PIECE) {
		return NO_PIECE_TYPE;
	}
	else if (piece == WHITE_KING || piece == BLACK_KING) {
		return KING;
	}
	return (PieceType)(piece % 6);
}

static inline int file_of(Square s){ return s % NUM_FILES; } // 0..7
static inline int rank_of(Square s){ return -((s - file_of(s)) / NUM_FILES) + 7; } // 0..7
static inline bool in_board(int f, int r){ return (f >= 0 ) && (f < NUM_FILES) && (r >= 0) && (r < NUM_RANKS); } // true if on board
static inline bool in_board_sq(Square sq){ return (file_of(sq) >= 0 ) && (file_of(sq) < NUM_FILES) && (rank_of(sq) >= 0) && (rank_of(sq) < NUM_RANKS); } // true if on board
static inline Square sq_of(int f,int r){ return (Square)(r*NUM_FILES + f); }
static inline Piece at(const Position* pos, Square s){ return pos->board[s]; }

static inline Piece promo_to_piece(Color us, char promoChar){
    if (us == WHITE){
        switch(promoChar){ case 'q': return WHITE_QUEEN; case 'r': return WHITE_ROOK;
                           case 'b': return WHITE_BISHOP; case 'n': return WHITE_KNIGHT; default: return WHITE_QUEEN; }
    } else {
        switch(promoChar){ case 'q': return BLACK_QUEEN; case 'r': return BLACK_ROOK;
                           case 'b': return BLACK_BISHOP; case 'n': return BLACK_KNIGHT; default: return BLACK_QUEEN; }
    }
}

// Move list
typedef struct { Move list[256]; int count; } MoveList;

static inline void add_move(MoveList* ml, Move m){
    ml->list[ml->count++] = m;
}



// FUNCTION PROTOTYPES
void init_position(Position *pos); // Initialize the position to the starting position
bool parse_fen(const char *fen, Position *pos); // TODO
char* position_to_fen(const Position *pos); // TODO
void print_board(const Position *pos); // Print the board with pieces

Move* parse_move(const char *move_str); // Parse a move from a string
int make_move(Position *pos, const Move *move); // Make a move on the board

bool is_square_attacked(const Position *pos, Square square, Color attacker);

#endif // TCHESS_H
