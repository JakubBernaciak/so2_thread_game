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
    else{
        printf("Witaj graczu: %d\n",id);
    }

    return 0;
}

