#include "server.h"

struct server_t server;

void* display(){
    
    while(server.online){
        clear();
        sem_wait(&server.sem);

        
        printw("%s",server.map);
        int row = 0;
        int col = 55;

        
        mvprintw(row++,col++,"Server's PID: %d",server.pid);
        mvprintw(row++,col,"Campsite X/Y: %d/%d",server.campsite_x,server.campsite_y);
        mvprintw(row++,col--,"Round number: %d",server.round_number++);
        row++;

        mvprintw(row++, col++, "Parameter: Player1 Player2 Player3 Player4");
        mvprintw(row++, col, "PID:");
        mvprintw(row++, col, "Curr X/Y:");
        mvprintw(row++, col--, "Deaths:");
        row++;
        mvprintw(row++,col++,"Coins");
        mvprintw(row++,col,"Carried:");
        mvprintw(row++,col,"Brought:");

        col+=10;
        row-=7;
        mvprintw(row++, col, "%s%8s%8s%8s","-","-","-","-");
        mvprintw(row++, col, "%s%8s%8s%8s","-","-","-","-");
        mvprintw(row++, col, "%s%8s%8s%8s","-","-","-","-");
        row+=2;
        mvprintw(row++, col, "%s%8s%8s%8s","-","-","-","-");
        mvprintw(row++, col, "%s%8s%8s%8s","-","-","-","-");

        row-=7;
        for(int i = 0 ; i < server.capacity_of_players ; i++ ){
            if(server.is_used[i] == 1){
                sem_wait(&server.players[i]->sem);
                mvprintw(row++,col + 8*i,"%d",server.players[i]->pid);
                mvprintw(row++,col + 8*i,"%d/%d",server.players[i]->x,server.players[i]->y);
                mvprintw(row++,col + 8*i,"%d",server.players[i]->deaths);
                row+=2;
                mvprintw(row++,col + 8*i,"%d",server.players[i]->c_carried);
                mvprintw(row++,col + 8*i,"%d",server.players[i]->c_brought);
                row-=7;
                sem_post(&server.players[i]->sem);
            }
        }
            
        sem_post(&server.sem);
        refresh();
        sleep(1);
    }
    return NULL;
}

int load_map(){
    server.map = calloc(27*54,sizeof(char));
    if(server.map == NULL)
        return 2;
    
    FILE *f = fopen("map.txt","rt");
    if(f == NULL){
        free(server.map);
        return 1;
    }
    fscanf(f,"%[^Q]",server.map);
    fclose(f);
    return 0;
}


void init_player(struct player_t *player,int id, int player_pid){

    sem_wait(&player->sem);
    player->server_pid = getpid();
    player->pid = player_pid;
    player->id = id;

    player->campsite_x = 0;
    player->campsite_y = 0;

    player->x = 0;
    player->y = 0;

    player->c_brought = 0;
    player->c_carried = 0;

    player->deaths = 0;
    sem_post(&player->sem);
}

void* add_player(void* arg){
    struct connection_t *connection = (struct connection_t *) arg;
    int id = connection->id;
    int player_pid = connection->player_pid;

    char name[10] = "player_";
    name[7] = (char)('0' + id);
    
    int shm_ID = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_ID,sizeof(struct player_t));

    sem_wait(&server.sem);
    *(server.players + id) = (struct player_t*)mmap(NULL, sizeof(struct player_t), PROT_READ | PROT_WRITE,MAP_SHARED,shm_ID,0);
    sem_init(&server.players[id]->sem,1,1);
    init_player(server.players[id], id,player_pid);
    sem_post(&server.sem);

    while(server.online){
        sem_wait(&server.players[id]->sem);
        sem_post(&server.players[id]->sem);
        sleep(1);
    }
    printf("%d %d %s\n",server.players[id]->pid,connection->id,name);
    
    sem_destroy(&server.players[id]->sem);
    shm_unlink(name);

    return NULL;
}

void* run_lobby(void* arg){
    sem_wait(&server.sem);
    int shm_ID = shm_open("lobby", O_CREAT | O_RDWR, 0666);
    if(shm_ID == -1){
        server.online = -1;
        sem_post(&server.sem);
        return NULL;
    }
    server.online = 1;
    ftruncate(shm_ID,sizeof(struct connection_t));
    sem_post(&server.sem);
    
    struct connection_t *connection = (struct connection_t *)mmap(NULL, sizeof(struct connection_t), PROT_READ | PROT_WRITE,MAP_SHARED,shm_ID,0);
    sem_init(&connection->sem,1,1);
    connection->used = 1;
    connection->id = -1;

    while(server.online){
        sem_wait(&connection->sem);
        
        if(connection->used == 1 && connection->id != -1){
            sem_wait(&server.sem);
            server.is_used[connection->id] = 1;
            server.size_of_players++;
            struct connection_t temp_connection = {.id = connection->id, .player_pid = connection->player_pid };
            pthread_create(&server.players_threads[connection->id], NULL, add_player, &temp_connection);
            sem_post(&server.sem);
        }

        connection->id = -1;
        connection->used = 0;

        sem_wait(&server.sem);    
        for(int i = 0 ; i < server.capacity_of_players ; i++){
            if(server.is_used[i] == 0){
                connection->id = i;
                break;
            }
        }
        sem_post(&server.sem);
                    
        sem_post(&connection->sem);
    }

    sem_destroy(&connection->sem);
    shm_unlink("lobby");

    return NULL;
}

struct server_t * init_server(){
    
    sem_init(&server.sem,1,1);
    server.pid = getpid();
    server.online = 0;

    server.capacity_of_players = 4;
    server.size_of_players = 0;

    for(int i = 0; i < server.capacity_of_players; i++){
        server.is_used[i] = 0;
    }

    server.round_number = 0;
    server.campsite_x = 24;
    server.campsite_y = 12;
    
    return &server;
}