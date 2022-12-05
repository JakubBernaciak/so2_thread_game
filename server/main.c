#include "server.h"

int main(){
    struct server_t *server = init_server();
    
    pthread_t lobby_thread;
    pthread_create(&lobby_thread, NULL, run_lobby, NULL);
    printf("Server pid: %d\n",getpid());

    while(1){
        sem_wait(&server->sem);
        if(server->online == -1){
            puts("Server is already running");
            sem_post(&server->sem);
            sem_destroy(&server->sem);
            return 0;
        }
        else if(server->online == 1){
            sem_post(&server->sem);
            break;
        }
        sem_post(&server->sem);
    }


    int err_code = load_map();
    if(err_code == 2){
        puts("Couldn't allocate memory for map");
        server->online = 0;
        return 2;
    }
    else if(err_code == 1){
        puts("Couldn't open file with map");
        server->online = 0;
        return 1;
    }
    initscr();
    pthread_t display_thread;
    pthread_create(&display_thread,NULL,start_game,NULL);

    while(1){
        int c = getch();
        if(c == 'q')
            break;
        if(c == 'c' || c == 't' || c == 'T')
            spawn_reward(c);
        if(c == 'b'){
            pthread_t thread;
            pthread_create(&thread,NULL,spawn_beast,NULL);
        }
    }
    server->online = 0;
    endwin();
    
    pthread_join(lobby_thread,NULL);

    return 0;
}