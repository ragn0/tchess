FLAGS = -Wall -Wextra -O0 -Wpedantic 
OUT = tchess
CC = gcc

$(OUT): main.c
	$(CC) $(FLAGS) -o $(OUT) main.c

clean:
	rm -f $(OUT)
run: $(OUT)
	./$(OUT)
