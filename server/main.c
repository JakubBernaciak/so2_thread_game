#include "server.h"

int main(){
    system("rm -rf /dev/shm/player_*"); 
    struct server_t server;
    init_server(&server);
    
    pthread_t lobby_thread;
    pthread_create(&lobby_thread, NULL,start_lobby,(&server));
    
    getchar();
    server.online =0;
    pthread_join(lobby_thread,NULL);
    close_server(&server);
    // start_lobby();
    return 0;
}