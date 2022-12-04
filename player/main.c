#include "player.h"

int main(){
    int id = get_id_from_server();
    if(id == -1){
        puts("Server is full");
        return 0;
    }
    else if(id == -2){
        puts("Server is offline");
        return 0;
    }

    printf("Witaj graczu: %d\npid:%d\n",id,getpid());
    struct player_t *player = connect_to_server(id);

    while(1){
        int c = getchar();
        if(c == 'q')
            break;
        if(c == 's'){
            sem_wait(&player->sem);
            player->x++;
            sem_post(&player->sem);
        }

    }


    return 0;
}

