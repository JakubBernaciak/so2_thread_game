#include "player.h"

int get_id_from_server(){
    key_t key = ftok("server",69);
    int shm_ID = shmget(key, sizeof(struct connection_t), 0666);
    if(shm_ID == -1){
        return -2;
    }

    struct connection_t *connection = (struct connection_t *)shmat(shm_ID, NULL, 0);
    sem_wait(&connection->sem);

    int id = connection->id;
    connection->used = 1;

    sem_post(&connection->sem);
    shmdt((void *) connection);

    return id;
}