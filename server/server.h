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
#include <signal.h>

#define MAX_NUMBER_OF_PLAYER 4
#define MAX_NUMBER_OF_BEASTS 4
#define MAX_NUMBER_OF_DROPS 20

struct server_t{
    sem_t sem;
    int pid;
    int online;
    char* map;
    
    int size_of_players;
    int capacity_of_players;
    int is_used[MAX_NUMBER_OF_PLAYER];
    struct player_t *players[MAX_NUMBER_OF_PLAYER];
    pthread_t players_threads[MAX_NUMBER_OF_PLAYER];
    
    int round_number;
    
    int campsite_x;
    int campsite_y;

    struct beast_t* beasts[MAX_NUMBER_OF_BEASTS];
    int number_of_beast;
    int capacity_of_beast;

    struct drop_t *drops;
    int drop_is_used[MAX_NUMBER_OF_DROPS];
    int drop_capacity;
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

struct drop_t{
    int x;
    int y;
    char under;
    int reward;
};


struct server_t * init_server();
void* run_lobby();
int load_map();
void get_map(struct player_t*);
void spawn_reward(char c);
void* start_game();
void display();
void close_server();

void init_player(struct player_t *player,int id, int player_pid);
void* add_player(void* arg);
void move_player(struct player_t*);
void interaction(struct player_t*);
void update_players();
void player_death(struct player_t * player);

void *spawn_beast();
void move_beast(struct beast_t *beast,int x,int y);
void beast_kill(struct beast_t *beast);
void update_beasts();
int detect_player(struct beast_t *beast);
int check_if_player(char c);
void campsite_update();

void add_drop(int reward, int x, int y, char under);
void get_drop(struct player_t * player);
int init_drops();

struct connection_t *create_connection(int id, int player_pid);
#endif //SERVER_H