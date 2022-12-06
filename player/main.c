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

    struct player_t *player = connect_to_server(id);
    pthread_t thread;
    initscr();
    pthread_create(&thread,NULL,display,player);
    while(player->online){
        int c = getch();
        if(c == 'q')
            break;
        switch(c){
            case 'w':
                player->move_y = -1;
                break;
            case 'a':
                player->move_x = -1;
                break;
            case 's':
                player->move_y = 1;
                break;
            case 'd':
                player->move_x = 1;
                break;
        }

    }
    player->online = 0;
    endwin();

    return 0;
}

