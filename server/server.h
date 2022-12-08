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

#define MAX_NUMBER_OF_PLAYERS 4
#define MAX_NUMBER_OF_BEASTS 4
#define MAX_NUMBER_OF_DROPS 20

typedef struct position_t{
    int x;
    int y;
} position_t;

typedef struct drop_t{
    position_t position;
    char under;
    int reward;
} drop_t;

struct server_t{
    sem_t sem;
    int pid;
    int online;
    char* map;
    
    int is_used[MAX_NUMBER_OF_PLAYERS];
    struct player_t *players[MAX_NUMBER_OF_PLAYERS];
    
    int round_number;
    
    position_t campsite;

    struct beast_t* beasts[MAX_NUMBER_OF_BEASTS];
    int number_of_beast;

    drop_t drops[MAX_NUMBER_OF_DROPS];
    int drop_is_used[MAX_NUMBER_OF_DROPS];
};

struct connection_t{
    sem_t sem;
    int used;
    int id;
    int player_pid;
};

struct beast_t{
    sem_t sem;
    position_t position;
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

void add_drop(int reward, position_t position, char under);
void get_drop(struct player_t * player);

struct connection_t *create_connection(int id, int player_pid);
position_t get_free_slot_on_map();
int is_server_online();
int is_player_online(struct player_t * player);

#endif //SERVER_H