#include <stdio.h>
#include <stdlib.h>
#include "tchess.h"

int main() {
	chessboard_t chessboard;
	init_chessboard(&chessboard);
	print_chessboard(&chessboard);
	return EXIT_SUCCESS;
}
