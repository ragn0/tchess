#define WHITE_ROOK 'R'
#define WHITE_KNIGHT 'N'
#define WHITE_BISHOP 'B'
#define WHITE_QUEEN 'Q'
#define WHITE_KING 'K'
#define WHITE_PAWN 'P'

#define BLACK_ROOK 'r'
#define BLACK_KNIGHT 'n'
#define BLACK_BISHOP 'b'
#define BLACK_QUEEN 'q'
#define BLACK_KING 'k'
#define BLACK_PAWN 'p'

#define EMPTY_SQUARE ' ' // Represents an empty square on the chessboard

typedef struct chessboard_t {
	char board[8][8]; // 8x8 chessboard
	unsigned char turn;        // 0 for white's turn, 1 for black
	unsigned char en_passant;  // 0 for no en passant, 1-8 for the column of the pawn that can be captured en passant
	unsigned char castling_w;  // 0 for no castling, 1 for white king-side, 2 for white queen-side, 3 for both
	unsigned char castling_b;  // 0 for no castling, 1 for black king-side, 2 for black queen-side, 3 for both
} chessboard_t;

typedef struct move_t {
	unsigned char from_row;    // 0-7 for the row of the piece being moved
	unsigned char from_col;    // 0-7 for the column of the piece being moved
	unsigned char to_row;      // 0-7 for the row where the piece is moved to
	unsigned char to_col;      // 0-7 for the column where the piece is moved to
	unsigned char promotion;    // 0 for no promotion, 1-3 for promotion to queen, rook, or bishop
} move_t;

typedef struct move_list_t {
	move_t moves[256];         // List of moves
	unsigned char count;       // Number of moves in the list
} move_list_t;

typedef struct game_state_t {
	chessboard_t board;        // Current chessboard
	move_list_t move_list;     // List of moves made in the game
	unsigned char halfmove_clock; // Halfmove clock for the fifty-move rule
	unsigned char fullmove_number; // Fullmove number in the game
} game_state_t;

void init_chessboard(chessboard_t *cb);
void print_chessboard(chessboard_t *cb);
void make_move(game_state_t *state, move_t move);
void undo_move(game_state_t *state);


