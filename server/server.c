#include "server.h"

struct server_t server;


void init_player(struct player_t *player, struct connection_t connection){
    sem_init(&player->sem,1,1);
    player->server_pid = getpid();
    player->pid = connection.player_pid;
    player->id = connection.id;

    player->campsite_x = 0;
    player->campsite_y = 0;

    player->x = 0;
    player->y = 0;

    player->c_brought = 0;
    player->c_carried = 0;

    player->deaths = 0;
}

void* add_player(void* arg){
    struct connection_t *connection = (struct connection_t *) arg;
    struct player_t player;
    init_player(&player,*connection);
    server.players[player.id] = &player;
    
    printf("New player arrived ID:%d PID:%d\n",player.id, player.pid);

    sem_destroy(&player.sem);

    return NULL;
}

void* run_lobby(void* arg){
    sem_wait(&server.sem);
    key_t key = ftok("server",69);
    int shm_ID = shmget(key, sizeof(struct connection_t), IPC_CREAT | IPC_EXCL | 0666);
    
    if(shm_ID == -1){
        server.online = -1;
        sem_post(&server.sem);
        return NULL;
    }

    server.online = 1;
    sem_post(&server.sem);
    
    struct connection_t *connection = (struct connection_t *)shmat(shm_ID, NULL, 0);
    sem_init(&connection->sem,1,1);
    connection->used = 1;
    connection->id = -1;

    while(server.online){
        sem_wait(&connection->sem);
        
        if(connection->used == 1 && connection->id != -1){
            sem_wait(&server.sem);
            server.is_used[connection->id] = 1;
            server.size_of_players++;
            pthread_t player_thread;
            struct connection_t temp_connection = {.id = connection->id, .player_pid = connection->player_pid };
            pthread_create(&player_thread, NULL, add_player, &temp_connection);
            sem_post(&server.sem);
        }

        connection->id = -1;
        connection->used = 0;

        sem_wait(&server.sem);    
        for(int i = 0 ; i < server.capacity_of_players ; i++){
            if(server.is_used[i] == 0){
                connection->id = i;
                break;
            }
        }
        sem_post(&server.sem);
                    
        sem_post(&connection->sem);
    }

    sem_destroy(&connection->sem);
    shmdt((void *) connection);
    shmctl(shm_ID, IPC_RMID, NULL);

    return NULL;
}

struct server_t * init_server(){
    
    sem_init(&server.sem,1,1);
    server.pid = getpid();
    server.online = 0;

    server.capacity_of_players = 4;
    server.size_of_players = 0;
    
    for(int i = 0; i < server.capacity_of_players; i++){
        server.is_used[i] = 0;
    }

    server.round_number = 0;
    server.campsite_x = 0;
    server.campsite_y = 0;
    
    return &server;
}