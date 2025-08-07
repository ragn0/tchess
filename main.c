#include <stdio.h>
#include "tchess.h"
#include <stdlib.h>

int main(){
	Position *pos = malloc(sizeof(Position));
	init_position(pos);
	for (int i = 0; i < NUM_SQUARES; i++) {
		printf("%d ", pos->board[i]);
	}
}
