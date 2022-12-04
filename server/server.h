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
#include <ncurses.h>

struct server_t{
    sem_t sem;
    int pid;
    int online;
    char* map;
    
    int size_of_players;
    int capacity_of_players;
    int is_used[4];
    struct player_t *players[4];
    pthread_t players_threads[4];
    
    int round_number;
    
    int campsite_x;
    int campsite_y;
};

struct connection_t{
    sem_t sem;
    int used;
    int id;
    int player_pid;
};

struct player_t{
    sem_t sem;
    int server_pid;
    int pid;
    int id;

    int campsite_x;
    int campsite_y;

    int x;
    int y;

    int c_carried;
    int c_brought;

    int deaths;
    int round;
};

struct server_t * init_server();
void* run_lobby(void* arg);
void* add_player(void* arg);
void init_player(struct player_t *player,int id, int player_pid);
void* display();
int load_map();

#endif //SERVER_H