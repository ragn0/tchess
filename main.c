#include "tchess.h"
#include "generators.h"
#include <stdlib.h>
#include <stdio.h>

int main(){
	Position *pos = malloc(sizeof(Position));
	MoveList *move_list = malloc(sizeof(MoveList));
	
	Move *parsed_move = parse_move("b1c3");
	Move *parsed_move3 = parse_move("e2e4");
	init_position(pos);
	print_board(pos);
	generate_pseudo_legal_moves(pos, move_list);
	printf("Generated %d moves:\n", move_list->count);
	for(int i = 0; i < move_list->count; i++){
		printf("%s%s ", square_to_string(move_list->list[i].from), square_to_string(move_list->list[i].to));
	}
	printf("\nMaking move e2e4\n");
	make_move(pos, parsed_move3);
	print_board(pos);
	generate_pseudo_legal_moves(pos, move_list);
	printf("Generated %d moves:\n", move_list->count);
	for(int i = 0; i < move_list->count; i++){
		printf("%s%s ", square_to_string(move_list->list[i].from), square_to_string(move_list->list[i].to));
	}
}
