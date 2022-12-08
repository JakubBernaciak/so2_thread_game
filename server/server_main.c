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
        sem_wait(&server->sem);
        server->online = 0;
        sem_post(&server->sem);
        close_server();
        return 2;
    }
    else if(err_code == 1){
        puts("Couldn't read map");
        sem_wait(&server->sem);
        server->online = 0;
        sem_post(&server->sem);
        close_server();
        return 1;
    }
    
    initscr();
    pthread_t game_thread;
    pthread_create(&game_thread,NULL,start_game,NULL);
    pthread_t beasts_threads[MAX_NUMBER_OF_BEASTS];
    while(1){
        int c = getch();
        if(c == 'q')
            break;
        if(c == 'c' || c == 't' || c == 'T')
            spawn_reward(c);
        if(c == 'b'){
            sem_wait(&server->sem);
            if(server->number_of_beast < MAX_NUMBER_OF_BEASTS){    
                pthread_create(&beasts_threads[server->number_of_beast],NULL,spawn_beast,NULL);  
            }
            sem_post(&server->sem);
        }
    }
    sem_wait(&server->sem);
    server->online = 0;
    sem_post(&server->sem);
    endwin();

    sem_wait(&server->sem);
    int size = server->number_of_beast;
    sem_post(&server->sem);
    for(int i = 0; i < size ; i++){
        pthread_join(beasts_threads[i],NULL);
    }
    
    pthread_join(lobby_thread,NULL);
    pthread_join(game_thread,NULL);
    close_server();

    return 0;
}