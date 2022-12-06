#include "server.h"

struct server_t server;

void update_beasts(){
    for(int i = 0 ; i < server.number_of_beast; i++){
        pthread_mutex_lock(&server.beasts[i]->sem);
        if(server.beasts[i]->can_move){
            server.beasts[i]->can_move--;
        }
        pthread_mutex_unlock(&server.beasts[i]->sem);
    }
}

void player_death(struct player_t * player){
    player->c_carried = 0;
    player->deaths++;
    do{
        player->y = rand()%22 + 2;
        player->x = rand()%48 + 2;
        player->under = server.map[player->x + player->y *54];
    }while(player->under != ' ');
    server.map[player->x + player->y *54] = '0' + player->id + 1; 
}
void beast_kill(struct beast_t *beast){
    int player_id = (int)(beast->under - '0' - 1);
    sem_wait(&server.players[player_id]->sem);
    beast->under = server.players[player_id]->under;
    player_death(server.players[player_id]);
    sem_post(&server.players[player_id]->sem);
}

void move_beast(struct beast_t *beast,int x,int y){
    if(x){
        if(server.map[beast->x + x + beast->y *54] != '|' && server.map[beast->x + x + beast->y *54] != '*'){
            server.map[beast->x + beast->y *54] = beast->under;
            beast->x += x;
            beast->under = server.map[beast->x + beast->y *54];
            if(beast->under >= '1' && beast->under <= '4'){
                beast_kill(beast);
            }
            server.map[beast->x + beast->y *54] = '*';
        }
    }
    else{
        if(server.map[beast->x + (beast->y + y)*54] != '|' && server.map[beast->x + (beast->y + y)*54] != '*'){
            server.map[beast->x + beast->y *54] = beast->under;
            beast->y += y;
            beast->under = server.map[beast->x + beast->y *54];
            if(beast->under >= '1' && beast->under <= '4'){
                beast_kill(beast);
            }
            server.map[beast->x + beast->y *54] = '*';
        }

    }
    beast->can_move = 1;
}
int check_if_player(char c){
    if(c >= '1' && c <= '4')
        return 1;
    return 0;
}

int detect_player(struct beast_t *beast){
    //insta kill
    if(check_if_player(server.map[beast->x + 1 + beast->y *54]))
        return 1;
    if(check_if_player(server.map[beast->x - 1 + beast->y *54]))
        return 2;
    if(check_if_player(server.map[beast->x + (beast->y + 1) *54]))
        return 3;
    if(check_if_player(server.map[beast->x + (beast->y - 1)  *54]))
        return 0;
    
    for(int i = -2 ;i <= 2 ; i++){
        for(int j = -2 ; j <= 2; j++){
            if(check_if_player(server.map[beast->x + i + (beast->y + j) *54])){
                if(i == 0){
                    if(j >0){
                        if(server.map[beast->x + (beast->y + 1) *54] != '|')
                            return 3;
                        continue;
                    }
                    if(server.map[beast->x + (beast->y - 1) *54] != '|')
                        return 0;
                    continue;
                }
                else if(j == 0){
                    if(i >0){
                        if(server.map[beast->x + 1 + (beast->y) *54] != '|')
                            return 1;
                        continue;
                    }
                    if(server.map[beast->x - 1 + (beast->y - 1) *54] != '|')
                        return 2;
                    continue;
                }
                else if(abs(i) == 1 && abs(j) == 1){
                    if(server.map[beast->x + i + (beast->y) *54] != '|'){
                        if(i > 0)
                            return 1;
                        return 2;
                    }
                    if(server.map[beast->x + (beast->y + j) *54] != '|'){
                        if(j > 0)
                            return 2;
                        return 0;
                    }
                }
                else if(abs(i) == 2 && abs(j) == 2){
                    int move_i = 1;
                    if(i < 0)
                        move_i = -1;
                    int move_j = 1;
                    if(j < 0)
                        move_j = -1;
        
                    if(server.map[beast->x + move_i + (beast->y + move_j) *54] != '|'){
                        if(server.map[beast->x + move_i + (beast->y) *54] != '|'){
                            if(i > 0)
                                return 1;
                            return 2;
                        }
                        if(server.map[beast->x + (beast->y + move_j) *54] != '|'){
                            if(j > 0)
                                return 2;
                            return 0;
                        }
                    }
                }
                else if(abs(i) == 2 && abs(j) == 1){
                    int move_i = 1;
                    if(i < 0)
                        move_i = -1;
                    int move_j = j;
                    if(server.map[beast->x + move_i + (beast->y + move_j) *54] != '|'){
                        if(server.map[beast->x + move_i + (beast->y) *54] != '|'){
                            if(i > 0)
                                return 1;
                            return 2;
                        }
                        if(server.map[beast->x + (beast->y + move_j) *54] != '|'){
                            if(j > 0)
                                return 2;
                            return 0;
                        }
                    }
                }
                else if(abs(i) == 1 && abs(j) == 2){
                    int move_j = 1;
                    if(j < 0)
                        move_j = -1;
                    int move_i = i;
                    if(server.map[beast->x + move_i + (beast->y + move_j) *54] != '|'){
                        if(server.map[beast->x + move_i + (beast->y) *54] != '|'){
                            if(i > 0)
                                return 1;
                            return 2;
                        }
                        if(server.map[beast->x + (beast->y + move_j) *54] != '|'){
                            if(j > 0)
                                return 2;
                            return 0;
                        }
                    }
                }

            }
        }
    }

    
    return -1;
}

void *spawn_beast(void *arg){
    srand(time(NULL));
    struct beast_t beast;
    beast.can_move = 1;
    beast.under = ' ';
    sem_wait(&server.sem);
    do{
        beast.y = rand()%22 + 2;
        beast.x = rand()%48 + 2;
        
    }while(server.map[beast.x + beast.y *54] != ' ');
    server.map[beast.x + beast.y *54] = '*';
    server.beasts[server.number_of_beast++] = &beast;
    sem_post(&server.sem);

    while(server.online){
        
        if(!beast.can_move){
            sem_wait(&server.sem);
            pthread_mutex_lock(&beast.sem);
            int los = detect_player(&beast);
            if(los == -1){
                los = rand()%4;
            }
            switch(los){
                case 1:
                    move_beast(&beast,1,0);
                    break;
                case 2:
                    move_beast(&beast,-1,0);
                    break;
                case 3:
                    move_beast(&beast,0,1);
                    break;
                case 0:
                    move_beast(&beast,0,-1);
                    break;
            }
            pthread_mutex_unlock(&beast.sem);
            sem_post(&server.sem);
        }
        
    }
    return NULL;
}

void spawn_reward(char c){
    sem_wait(&server.sem);
    srand(time(NULL));
    int x;
    int y;
    do{
        y = rand()%22 + 2;
        x = rand()%48 + 2;
        
    }while(server.map[x + y *54] != ' ');

    server.map[x + y *54] = c;
    sem_post(&server.sem);
}
void update_players(){
    for(int i = 0 ; i < server.capacity_of_players ; i++ ){
        if(server.is_used[i] == 1){
            sem_wait(&server.players[i]->sem);
            if(server.players[i]->can_move)
                server.players[i]->can_move--;
            server.players[i]->round = server.round_number;
            get_map(server.players[i]);
            sem_post(&server.players[i]->sem);
        }
    }
}

void* start_game(){
    while(server.online){
        sem_wait(&server.sem);
        server.round_number++;
        display();
        update_beasts();
        update_players();
        sem_post(&server.sem);
        usleep(200000);
    }
    return NULL;
}

void display(){
    clear();
    printw("%s",server.map);
    int row = 0;
    int col = 55;
    
    mvprintw(row++,col++,"Server's PID: %d",server.pid);
    mvprintw(row++,col,"Campsite X/Y: %d/%d",server.campsite_x,server.campsite_y);
    mvprintw(row++,col--,"Round number: %d",server.round_number);

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
    mvprintw(100,100," ");
    refresh(); 
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
    srand(time(NULL));
    sem_wait(&player->sem);
    player->online = 0;
    player->server_pid = getpid();
    player->pid = player_pid;
    player->id = id;
    player->can_move = 0;
    player->move_x = 0;
    player->move_y = 0;

    player->campsite_x = 0;
    player->campsite_y = 0;

    do{
        player->y = rand()%22 + 2;
        player->x = rand()%48 + 2;
        player->under = server.map[player->x + player->y *54];
    }while(player->under != ' ');

    server.map[player->x + player->y *54] = '0' + player->id +1;

    player->c_brought = 0;
    player->c_carried = 0;

    player->deaths = 0;
    sem_post(&player->sem);
}

void get_map(struct player_t* player){
    for(int i = -2 ;i <= 2 ; i++){
        for(int j = -2 ; j <= 2; j++){
            player->map[(j+2)*5+(i+2)] = server.map[player->x + i + (player->y +j) *54];
            if(player->map[(j+2)*5+(i+2)] == 'A'){
                player->campsite_x = server.campsite_x;
                player->campsite_y = server.campsite_y;
            }
            player->online = 1;
        }
    }
}

void interaction(struct player_t* player){
    switch(player->under){
        case '#':
            player->can_move++;
            break;
        case 'c':
            player->c_carried ++;
            player->under = ' ';
            break;
        case 't':
            player->c_carried += 10;
            player->under = ' ';
            break;
        case 'T':
            player->c_carried += 50;
            player->under = ' ';
            break;
        case 'D':
            player->under = ' ';
            break;
        case 'A':
            player->c_brought += player->c_carried;
            player->c_carried = 0;
            break;
    }

}
void move_player(struct player_t* player){ 
    if(!player->can_move){
        if(player->move_x != 0){
            sem_wait(&server.sem);
            if(server.map[player->x + player->move_x + player->y *54] == '*'){
                server.map[player->x + player->y *54] = player->under;
                player_death(player);
                player->can_move ++;
            }
            else if(check_if_player(server.map[player->x + player->move_x + (player->y  ) *54])){
                int player_id = (int)(server.map[player->x + player->move_x + (player->y) *54] - '0' - 1);
                sem_wait(&server.players[player_id]->sem);
                player_death(server.players[player_id]);
                sem_post(&server.players[player_id]->sem);
                server.map[player->x + player->move_x + (player->y) *54] = 'D';
                server.map[player->x + (player->y) *54] = player->under;
                player_death(player);
                player->can_move ++;
            }
            else if(server.map[player->x + player->move_x + player->y *54] != '|')
            {
                    server.map[player->x + player->y *54] = player->under;
                    player->x += player->move_x;
                    player->under = server.map[player->x + player->y *54];
                    server.map[player->x + player->y *54] = '0' +player->id + 1;
                    player->can_move ++;
                    interaction(player);
            } 
            sem_post(&server.sem);
            
        }
        else if(player->move_y){
            sem_wait(&server.sem);
            if(server.map[player->x + (player->y + player->move_y ) *54] == '*'){
                server.map[player->x + player->y *54] = player->under;
                player_death(player);
                player->can_move ++;
            }
            else if(check_if_player(server.map[player->x + (player->y + player->move_y ) *54])){
                int player_id = (int)(server.map[player->x + (player->y + player->move_y ) *54] - '0' - 1);
                sem_wait(&server.players[player_id]->sem);
                player_death(server.players[player_id]);
                sem_post(&server.players[player_id]->sem);
                server.map[player->x + (player->y + player->move_y) *54] = 'D';
                server.map[player->x + (player->y) *54] = player->under;
                player_death(player);
                player->can_move ++;
            }
            else if(server.map[player->x + (player->y + player->move_y ) *54] != '|'){
                server.map[player->x + player->y *54] = player->under;
                player->y += player->move_y;
                player->under = server.map[player->x + player->y *54];
                server.map[player->x + player->y *54] = '0' +player->id + 1;
                player->can_move ++;
                interaction(player);
            }
            sem_post(&server.sem);
        }
    }
    player->move_x = 0;
    player->move_y = 0;
    
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
    get_map(server.players[id]);
    sem_post(&server.sem);

    while(server.online && server.players[id]->online){
        sem_wait(&server.players[id]->sem);
        if(kill(server.players[id]->pid, 0) == -1){
            sem_post(&server.players[id]->sem);
            break;
        }
        move_player(*(server.players + id));
        sem_post(&server.players[id]->sem);
    }
    server.map[server.players[id]->x + (server.players[id]->y) *54] = server.players[id]->under;
    server.is_used[id] = 0;
    sem_destroy(&server.players[id]->sem);
    server.players[id] = NULL;
    server.size_of_players --;
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

    server.number_of_beast = 0;
    server.capacity_of_beast = 10;
    
    return &server;
}
void close_server(){
    sem_wait(&server.sem);
    free(server.map);
    server.map = NULL;
    sem_post(&server.sem);
    sem_destroy(&server.sem);
}