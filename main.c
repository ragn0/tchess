#include "tchess.h"
#include "generators.h"
#include <stdlib.h>
#include <stdio.h>

int main(){
	Position *pos = malloc(sizeof(Position));
	MoveList *move_list = malloc(sizeof(MoveList));
	
	init_position(pos);
	while (1) {
		print_board(pos);
		char* input = malloc(6 * sizeof(char));
		printf("Enter your move (e.g., e2e4): ");
		scanf("%5s", input);
		if (input[0] == 'q') break; // Quit if user inputs 'q'
		Move *move = parse_move(input);
		if (move) {
			make_move(pos, move);
			generate_pseudo_legal_moves(pos, move_list);
		}
		for (int i = 0; i < move_list->count; i++) {
			printf("%s%s ", square_to_string(move_list->list[i].from), square_to_string(move_list->list[i].to));
		}

	}
}
