#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

#include <pigpio.h>

#define PORT 3581
#define BUFFER_SIZE 1024

#define ADR_I2C 0x70
#define ADR_SYS_MATRICE 0x20
#define ADR_AFFICHAGE_MATRICE 0x80

//test : nc 10.10.21.5 3581

int main() {
    int socket_local, socket_dist;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    int handle;

    int numberForMat = 0;
    bool isTeam1On = false;
    bool isTeam2On = false;
    bool isTeam3On = false;
    bool isTeam4On = false;
    bool isTeam5On = false;
    bool isTeam6On = false;
    bool isTeam7On = false;
    bool isTeam8On = false;
    
    bool isTeam1AlreadyAdded = false;
    bool isTeam2AlreadyAdded = false;
    bool isTeam3AlreadyAdded = false;
    bool isTeam4AlreadyAdded = false;
    bool isTeam5AlreadyAdded = false;
    bool isTeam6AlreadyAdded = false;
    bool isTeam7AlreadyAdded = false;
    bool isTeam8AlreadyAdded = false;

    char * delimiter = ":";
    char * onOrOff;
    char * equipe;
    char * valuesSepareted;

    // Créer le socket et initialiser l'adresse
    socket_local = socket(AF_INET, SOCK_STREAM, 0);
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Initialiser pigpio
    if (gpioInitialise() < 0) {
        printf("Erreur d'initialisation pigpio\n");
        return 1;
    }

    // Récupérer le référence ("handle") de la matrice
    handle = i2cOpen(1, ADR_I2C, 0);
    
    if (handle < 0) {
        printf("Erreur d'accès à la matrice LED\n");
        gpioTerminate();
        return 1;
    }

    // Initialiser la matrice
    i2cWriteByteData(handle, ADR_SYS_MATRICE | 1, 0x00);
    i2cWriteByteData(handle, ADR_AFFICHAGE_MATRICE | 1, 0x00);

    // Associer le socket à l'adresse de l'interface
    bind(socket_local, (struct sockaddr *)&address, sizeof(address));
    
    // Attendre une connexion entrante
    listen(socket_local, 3);
    socket_dist = accept(socket_local, (struct sockaddr *)&address, (socklen_t*)&addrlen);


    while(1) {
        int datalen;
        // Stocker le message
        datalen = read(socket_dist, buffer, BUFFER_SIZE);
        if (datalen == 0) {
            printf("Déconnexion\n");
            break;
        } else { // Afficher le message
            printf("Reçu: %s", buffer);
            valuesSepareted = strtok(buffer, delimiter);
            int comptor = 0;
            while( valuesSepareted != NULL ) {
                if(comptor == 0) {
                    equipe = valuesSepareted;
                }
                if(comptor == 1) {
                    onOrOff = valuesSepareted;
                }
                printf( " %s\n", valuesSepareted );

                valuesSepareted = strtok(NULL, delimiter);

                comptor +=1;
            }
            printf("equipe : %s\n", equipe);
            printf("onOrOff : %s\n", onOrOff);
            if(strcmp(onOrOff,"0\n") == 0 || strcmp(onOrOff,"1\n") == 0) {
                if(strcmp(onOrOff,"0\n") == 0){
                    if(strcmp(equipe, "1") == 0 && !isTeam1AlreadyAdded) {
                        numberForMat += 1;
                        isTeam1On = true;
                        isTeam1AlreadyAdded = true;
                    }
                    else if(strcmp(equipe, "2") == 0 && !isTeam2AlreadyAdded) {
                        numberForMat += 2;
                        isTeam2On = true;
                        isTeam2AlreadyAdded = true;
                    }
                    else if(strcmp(equipe, "3") == 0 && !isTeam3AlreadyAdded) {
                        numberForMat += 4;
                        isTeam3On = true;
                        isTeam3AlreadyAdded = true;
                    }
                    else if(strcmp(equipe, "4") == 0 && !isTeam4AlreadyAdded) {
                        numberForMat += 8;
                        isTeam4On = true;
                        isTeam4AlreadyAdded = true;
                    }
                    else if(strcmp(equipe, "5") == 0 && !isTeam5AlreadyAdded) {
                        numberForMat += 16;
                        isTeam5On = true;
                        isTeam5AlreadyAdded = true;
                    }
                    else if(strcmp(equipe, "6") == 0 && !isTeam6AlreadyAdded) {
                        numberForMat += 32;
                        isTeam6On = true;
                        isTeam6AlreadyAdded = true;
                    }
                    else if(strcmp(equipe, "7") == 0 && !isTeam7AlreadyAdded) {
                        numberForMat += 64;
                        isTeam7On = true;
                        isTeam7AlreadyAdded = true;
                    }
                    else if(strcmp(equipe, "8") == 0 && !isTeam8AlreadyAdded) {
                        numberForMat += 128;
                        isTeam8On = true;
                        isTeam8AlreadyAdded = true;
                    }
                    else {
                        if(strcmp(equipe, "1") == 0 || 
                        strcmp(equipe, "2") == 0 ||
                        strcmp(equipe, "3") == 0 ||
                        strcmp(equipe, "4") == 0 ||
                        strcmp(equipe, "5") == 0 ||
                        strcmp(equipe, "6") == 0 ||
                        strcmp(equipe, "7") == 0 ||
                        strcmp(equipe, "8") == 0) {
                            printf("LED for team %s already on\n", equipe);
                        } else {
                            printf("Equipe out of range\n");
                            printf("Need to be between 1 and 8\n");
                        }
                    }
                    //printf("Go In if");
                } else {
                    if(strcmp(equipe, "1") == 0 && isTeam1AlreadyAdded && isTeam1On) {
                        numberForMat -= 1;
                        isTeam1AlreadyAdded = false;
                        isTeam1On = false;
                    }
                    else if(strcmp(equipe, "2") == 0 && isTeam2AlreadyAdded && isTeam2On) {
                        numberForMat -= 2;
                        isTeam2AlreadyAdded = false;
                        isTeam2On = false;
                    }
                    else if(strcmp(equipe, "3") == 0 && isTeam3AlreadyAdded && isTeam3On) {
                        numberForMat -= 4;
                        isTeam3AlreadyAdded = false;
                        isTeam3On = false;
                    }
                    else if(strcmp(equipe, "4") == 0 && isTeam4AlreadyAdded && isTeam4On) {
                        numberForMat -= 8;
                        isTeam4AlreadyAdded = false;
                        isTeam4On = false;
                    }
                    else if(strcmp(equipe, "5") == 0 && isTeam5AlreadyAdded && isTeam5On) {
                        numberForMat -= 16;
                        isTeam5AlreadyAdded = false;
                        isTeam5On = false;
                    }
                    else if(strcmp(equipe, "6") == 0 && isTeam6AlreadyAdded && isTeam6On) {
                        numberForMat -= 32;
                        isTeam6AlreadyAdded = false;
                        isTeam6On = false;
                    }
                    else if(strcmp(equipe, "7") == 0 && isTeam7AlreadyAdded && isTeam7On) {
                        numberForMat -= 64;
                        isTeam7AlreadyAdded = false;
                        isTeam7On = false;
                    }
                    else if(strcmp(equipe, "8") == 0 && isTeam8AlreadyAdded && isTeam8On) {
                        numberForMat -= 128;
                        isTeam8AlreadyAdded = false;
                        isTeam8On = false;
                    }
                    else {
                        if(strcmp(equipe, "1") == 0 || 
                        strcmp(equipe, "2") == 0 ||
                        strcmp(equipe, "3") == 0 ||
                        strcmp(equipe, "4") == 0 ||
                        strcmp(equipe, "5") == 0 ||
                        strcmp(equipe, "6") == 0 ||
                        strcmp(equipe, "7") == 0 ||
                        strcmp(equipe, "8") == 0) {
                            printf("LED for team %s already off\n", equipe);
                        } else {
                            printf("Equipe out of range\n");
                            printf("Need to be between 1 and 8\n");
                        }

                    }
                    //printf("Go In else");
                }
                printf("comptor : %i\n",numberForMat);
                i2cWriteByteData(handle, 0x00, numberForMat);
            } else {
                printf("On or off out of range");
                printf("Need to be 1 or 2");
            }

            fflush(stdout); 
        }
        memset(buffer, 0, BUFFER_SIZE); 
    }

    close(socket_dist);
    close(socket_local);
    return 0;
}