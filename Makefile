CC := gcc

PLAYER_FILES:= player/*.c player/*.h
SERVER_FILES := server/*.h server/*.c
OBJS := player.o server.o

FLAGS := -g -lpthread -lrt -lncurses #-pedantic #-Wall -Wextra #-Werror -Wfatal-errors -O1
OFLAGS := #-fsanitize=thread -fsanitize-recover=address -fno-omit-frame-pointer -fsanitize=undefined


all: $(OBJS)

player.o: $(PLAYER_FILES)
	$(CC) -o player.o $(PLAYER_FILES) $(FLAGS) $(OFLAGS)

server.o: $(SERVER_FILES)
	$(CC) -o server.o $(SERVER_FILES) $(FLAGS) $(OFLAGS)

clean:
	rm -rf /dev/shm/lobby
	rm -rf /dev/shm/player*
	rm -rf *.o
p:
	./player.o 2>error_log_player.txt
s:
	./server.o 2>error_log_server.txt