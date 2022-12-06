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
#include <time.h>
#include <math.h>

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

    struct beast_t* beasts[10];
    int number_of_beast;
    int capacity_of_beast;
    int killed[4];
};

struct connection_t{
    sem_t sem;
    int used;
    int id;
    int player_pid;
};

struct beast_t{
    pthread_mutex_t sem;
    int x;
    int y;
    int can_move;
    char under;
};

struct player_t{
    sem_t sem;
    int ready;
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

struct server_t * init_server();
void* run_lobby(void* arg);
int load_map();
void get_map(struct player_t*);
void spawn_reward(char c);
void* start_game();
void display();

void init_player(struct player_t *player,int id, int player_pid);
void* add_player(void* arg);
void move_player(struct player_t*);
void interaction(struct player_t*);
void update_players();
void player_death(struct player_t * player);

void *spawn_beast(void *arg);
void move_beast(struct beast_t *beast,int x,int y);
void beast_kill(struct beast_t *beast);
void update_beasts();
int detect_player(struct beast_t *beast);
int check_if_player(char c);

#endif //SERVER_H