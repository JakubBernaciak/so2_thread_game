#include "player.h"

int get_id_from_server(){
    int shm_ID = shm_open("lobby", O_RDWR, 0666);
    if(shm_ID == -1){
        return -2;
    }

    struct connection_t *connection = (struct connection_t *)mmap(NULL, sizeof(struct connection_t), PROT_READ | PROT_WRITE,MAP_SHARED,shm_ID,0);
    int id;
    int flag = 1;
    while(flag){
        sem_wait(&connection->sem);
        if(connection->used == 0){
            id = connection->id;
            connection->used = 1;
            connection->player_pid = getpid();
            flag = 0;
        }
        sem_post(&connection->sem);
    }

    shmdt((void *) connection);

    return id;
}


struct player_t* connect_to_server(int id){
    char name[10] = "player_";
    name[7] = (char)('0' + id);
    key_t key = ftok(name,69);
    int shm_ID;
    do{
        shm_ID = shm_open(name, O_RDWR, 0666);
    }while(shm_ID == -1);

    struct player_t *player = (struct player_t*)mmap(NULL, sizeof(struct player_t), PROT_READ | PROT_WRITE,MAP_SHARED,shm_ID,0);

    return player;
}