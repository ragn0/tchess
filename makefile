FLAGS = -std=c11 -Wall -Wextra -O0 -Wpedantic 
CC = gcc
OBJ = main.o tchess.o generators.o

all: tchess

tchess: $(OBJ)
	$(CC) $(FLAGS) $^ -o $(OUT) $@ 
%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o tchess
run: $(OUT)
	./$(OUT)
