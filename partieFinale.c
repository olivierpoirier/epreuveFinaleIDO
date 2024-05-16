#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <stdbool.h>

#include <pigpio.h>

#define PORT_SEND_DATA 2291
#define DEST_IP "10.10.0.185"
#define MESSAGE0 "0"
#define MESSAGE1 "1"

#define PINBUTTON 17
//test : nc -lp 2291


#define PORT_RECEIVE_DATA 6543
#define BUFFER_SIZE 1024

#define ADR_I2C 0x70
#define ADR_SYS_MATRICE 0x20
#define ADR_AFFICHAGE_MATRICE 0x80

int sock = 0;


void *partieBtn() {
    bool btnHasBeenClicked = false;
    while(1) {
        if(gpioRead(PINBUTTON) == 0 && !btnHasBeenClicked) {
            send(sock, MESSAGE0, strlen(MESSAGE0), 0);
            btnHasBeenClicked = true;
            printf("Sent 0 with TCP\n");
        } else {
            if(gpioRead(PINBUTTON) == 1 && btnHasBeenClicked) {
                int number = send(sock, MESSAGE1, strlen(MESSAGE1), 0);
                btnHasBeenClicked = false;
                printf("Sent 1 with TCP\n");
                //printf("%i", number);
            }
            //printf("con : %i\n", con);
        }
    }
}

void *partieMat() {

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

    int socket_local, socket_dist;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    socket_local = socket(AF_INET, SOCK_STREAM, 0);
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT_RECEIVE_DATA);


    // Récupérer le référence ("handle") de la matrice
    handle = i2cOpen(1, ADR_I2C, 0);
    
    if (handle < 0) {
        printf("Erreur d'accès à la matrice LED\n");
        gpioTerminate();
        return NULL;
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
            if(strcmp(onOrOff,"0") == 0 || strcmp(onOrOff,"1") == 0) {
                if(strcmp(onOrOff,"0") == 0){
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
                printf("On or off out of range\n");
                printf("Need to be  or 1\n");
            }

            fflush(stdout); 
        }
        memset(buffer, 0, BUFFER_SIZE); 
    }
    close(socket_dist);
    close(socket_local);
}



int main() {

    struct sockaddr_in dest_addr;
    
    // Créer le socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&dest_addr, '0', sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    dest_addr.sin_port = htons(PORT_SEND_DATA);


    if (gpioInitialise() < 0) {
        fprintf(stderr, "Erreur d'initialisation pigpio\n");
        return 1;
    }
    gpioSetMode(PINBUTTON, PI_INPUT);

    // Créer la connexion
    int con = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    printf("Ready To send data!\n");

    pthread_t t_button, t_mat;

    if (pthread_create(&t_button,NULL,partieBtn,NULL) != 0) {
        printf("Erreur à la création du thread pour le bouton. \n");
        return 1;
    }
    if (pthread_create(&t_mat,NULL,partieMat,NULL) != 0) {
        printf("Erreur à la création du thread pour la matrice.\n");
        return 1;
    }

    // Attendre la fin de l'exécution de chaque thread
    pthread_join(t_button, NULL);
    pthread_join(t_mat, NULL);
    close(sock);

    gpioTerminate();
    return 0;
}