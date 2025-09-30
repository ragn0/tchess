#include "tchess.h"
#include "generators.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
	Position *pos = malloc(sizeof(Position));
	MoveList *move_list = malloc(sizeof(MoveList));
	
	init_position(pos);
	while (1) {
		print_board(pos);
		char* input = malloc(6 * sizeof(char));
		generate_legal(pos, move_list);
		for (int i = 0; i < move_list->count; i++) {
					printf("%s%s ", square_to_string(move_list->list[i].from), square_to_string(move_list->list[i].to));
		}
		printf("\n");
		printf("Enter your move (e.g., e2e4): ");
		scanf("%5s", input);
		if (strncmp(input, "q", 1) != 0 && strlen(input) < 4) {
			printf("Invalid input. Try again.\n");
			continue;
		}
		if (input[0] == 'q') break; // Quit if user inputs 'q'
		Move *move = parse_move(input);
		for (int i = 0; i < move_list->count; i++) {
			if (move->promotionPiece == '0') {
			if (move_list->list[i].from == move->from && move_list->list[i].to == move->to) {
				make_move(pos, &move_list->list[i]);
				break;
			} else if (i == move_list->count - 1) {
				printf("Invalid move. Try again.\n");
				continue;
			}
			} else {
				if (move_list->list[i].from == move->from && move_list->list[i].to == move->to && move_list->list[i].promotionPiece == move->promotionPiece) {
					make_move(pos, &move_list->list[i]);
					break;
				} else if (i == move_list->count - 1) {
					printf("Invalid move. Try again.\n");
					continue;
				}
			}
		}
		

	}
}
