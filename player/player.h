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

struct player_t{
    sem_t sem;
    int online;
    int can_move;
    int server_pid;
    int pid;
    int id;

    int campsite_x;
    int campsite_y;

    int x;
    int y;
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

#endif //PLAYER_H