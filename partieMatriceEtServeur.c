#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pigpio.h>

#define PORT 3581
#define BUFFER_SIZE 1024

int main() {
    int socket_local, socket_dist;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Créer le socket et initialiser l'adresse
    socket_local = socket(AF_INET, SOCK_STREAM, 0);
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (gpioInitialise() < 0) {
        fprintf(stderr, "Erreur d'initialisation pigpio\n");
        return 1;
    }

    // Associer le socket à l'adresse de l'interface
    bind(socket_local, (struct sockaddr *)&address, sizeof(address));
    
    // Attendre une connexion entrante
    listen(socket_local, 3);
    socket_dist = accept(socket_local, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    char * delimiter = ";";
    char * equipe;
    char * boolOnOrOff;
    char dividedResponse; //pas utilisée
    char * teamLedOn[10];
    while(1) {
        int datalen;
        // Stocker le message
        datalen = read(socket_dist, buffer, BUFFER_SIZE);
        if (datalen == 0) {
            printf("Déconnexion\n");
            break;
        } else { // Afficher le message
            printf("Reçu: %s", buffer);
            equipe = strtok(buffer, delimiter);
            boolOnOrOff = strtok(buffer, delimiter);
            if(strcmp(boolOnOrOff,"0") == 0){
                
            } else {
                for (int i = 0; i < sizeof(teamLedOn); i++) { 
                    if(strcmp(teamLedOn[i],equipe) != 0){
                        teamLedOn[i] = teamLedOn[i +1] ;
                    }
                } 
            }
            fflush(stdout); 
        }
        memset(buffer, 0, BUFFER_SIZE); 
    }

    close(socket_dist);
    close(socket_local);
    return 0;
}