#include "tchess.h"
#include <stdlib.h>
#include <stdio.h>

int main(){
	Position *pos = malloc(sizeof(Position));
	Move *parsed_move = parse_move("b1c3");
	Move *parsed_move1 = parse_move("b1c3");
	Move *parsed_move2 = parse_move("b8c6");
	Move *parsed_move3 = parse_move("e2e4");
	printf("Parsed move: from %s to %s\n", square_to_string(parsed_move->from), square_to_string(parsed_move->to));
	init_position(pos);
	make_move(pos, parsed_move);
	print_board(pos);
	make_move(pos, parsed_move1);
	print_board(pos);
	make_move(pos, parsed_move2);
	print_board(pos);
	make_move(pos, parsed_move3);
	print_board(pos);
}
