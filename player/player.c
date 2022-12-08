#include "player.h"
int is_player_online(struct player_t * player){
    sem_wait(&player->sem);
    int res = player->online;
    sem_post(&player->sem);
    return res;
}

void *display(void *arg){
    struct player_t *player = (struct player_t*) arg;

    while(!is_player_online(player));

    while(is_player_online(player)){
        sem_wait(&player->sem);
        clear();
        for(int i = 0; i<25 ; i++){
            printw("%c",player->map[i]);
            if(i%5 ==4)
                printw("\n");
        }
        int row = 0;
        int col = 35;

        mvprintw(row++,col++,"Server's PID: %d",player->server_pid);
        mvprintw(row++,col,"Campsite X/Y: %d/%d",player->campsite.x,player->campsite.y);
        mvprintw(row++,col--,"Round number: %d",player->round);
        row++;

        mvprintw(row++, col++, "Player:");
        mvprintw(row++, col, "Number: %d",player->id +1);
        mvprintw(row++, col, "Curr X/Y: %d/%d",player->position.x,player->position.y);
        mvprintw(row++, col--, "Deaths: %d",player->deaths);
        row++;
        mvprintw(row++,col++,"Coins");
        mvprintw(row++,col,"Carried: %d",player->c_carried);
        mvprintw(row++,col,"Brought: %d",player->c_brought);
        
        sem_post(&player->sem);
        refresh();
        // usleep(200000);
    }
    return NULL;
}

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
    int shm_ID;
    do{
        shm_ID = shm_open(name, O_RDWR, 0666);
    }while(shm_ID == -1);

    struct player_t *player = (struct player_t*)mmap(NULL, sizeof(struct player_t), PROT_READ | PROT_WRITE,MAP_SHARED,shm_ID,0);

    return player;
}