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

    getchar();
    printf("%d\n",server->players[0]->pid);
    printf("%d\n",server->players[1]->pid);
    printf("%d\n",server->players[2]->pid);
    printf("%d\n",server->players[3]->pid);

    server->online = 0;
    pthread_join(lobby_thread,NULL);
    destroy_players();

    return 0;
}