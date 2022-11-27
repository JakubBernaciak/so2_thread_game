CC := gcc

PLAYER_FILES:= player/*.c
SERVER_FILES := server/*.h
OBJS := player.o server.o

FLAGS := -Wall -g -pthread
OFLAGS :=


all: $(OBJS)

player.o: $(PLAYER_FILES)
	$(CC) -o player.o $(PLAYER_FILES) $(FLAGS) $(OFLAGS)

server.o: $(SERVER_FILES)
	$(CC) -o server.o $(SERVER_FILES) $(FLAGS) $(OFLAGS)

clean:
	rm *.o