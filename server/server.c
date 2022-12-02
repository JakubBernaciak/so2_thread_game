#include "server.h"

void* run_lobby(void* arg){
    struct server_t *server = (struct server_t*)arg;
    key_t key = ftok("server",69);
    int shm_ID = shmget(key, sizeof(struct connection_t), IPC_CREAT | 0666);
    
    struct connection_t *connection = (struct connection_t *)shmat(shm_ID, NULL, 0);
    connection->used = 1;
    sem_init(&connection->sem,1,1);
    connection->id = -1;

    while(server->online){
        sem_wait(&connection->sem);
        
        if(connection->used == 1 && connection->id != -1){
            server->is_used[connection->id] = 1;
            server->size_of_players++;
        }

        connection->id = -1;
        connection->used = 0;
                
        for(int i = 0 ; i < server->capacity_of_players ; i++){
            if(server->is_used[i] == 0){
                connection->id = i;
                break;
            }
        }
                    
        sem_post(&connection->sem);
    }

    sem_destroy(&connection->sem);
    shmdt((void *) connection);
    shmctl(shm_ID, IPC_RMID, NULL);

    return NULL;
}

int init_server(struct server_t *server){
    server->server_pid = getpid();
    server->online = 1;

    server->capacity_of_players = 4;
    server->size_of_players = 0;
    
    for(int i = 0; i < server->capacity_of_players; i++){
        server->is_used[i] = 0;
    }

    server->round_number = 0;
    server->campsite_x = 0;
    server->campsite_y = 0;

    return 0;
}