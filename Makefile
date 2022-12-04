CC := gcc

PLAYER_FILES:= player/*.c player/*.h
SERVER_FILES := server/*.h server/*.c
OBJS := player.o server.o

FLAGS := -g -lpthread -lrt -lncurses
OFLAGS :=


all: $(OBJS)

player.o: $(PLAYER_FILES)
	$(CC) -o player.o $(PLAYER_FILES) $(FLAGS) $(OFLAGS)

server.o: $(SERVER_FILES)
	$(CC) -o server.o $(SERVER_FILES) $(FLAGS) $(OFLAGS)

clean:
	rm *.o
p:
	./player.o
s:
	./server.o