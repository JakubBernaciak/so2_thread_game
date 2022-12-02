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

struct connection_t{
    sem_t sem;
    int used;
    int id;
};

int get_id_from_server();

#endif //PLAYER_H