#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <ncurses.h>

typedef struct position_t{
    int x;
    int y;
} position_t;

struct player_t{
    sem_t sem;
    int online;
    int can_move;
    int server_pid;
    int pid;
    int id;

    position_t campsite;
    position_t position;
    
    int move_x;
    int move_y;

    int c_carried;
    int c_brought;

    int deaths;
    int round;
    char under;
    char map[25];
};

struct connection_t{
    sem_t sem;
    int used;
    int id;
    int player_pid;
};

int get_id_from_server();
struct player_t* connect_to_server(int id);
void *display(void *arg);
int is_player_online(struct player_t * player);

#endif //PLAYER_H