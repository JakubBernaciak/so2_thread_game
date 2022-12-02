#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>

struct server_t{
    int server_pid;
    int online;
    
    int size_of_players;
    int capacity_of_players;
    int is_used[4];
    struct player_t *players[4];
    
    int round_number;
    
    int campsite_x;
    int campsite_y;
};

struct connection_t{
    sem_t sem;
    int used;
    int id;
};

struct player_t{
    int server_pid;
    int player_pid;
    int player_id;

    int campsite_x;
    int campsite_y;

    int x;
    int y;

    int c_carried;
    int c_brought;

    int deaths;
    int round;
};

int init_server(struct server_t *server);

void* run_lobby(void* arg);

void close_server(struct server_t* server);

#endif //SERVER_H