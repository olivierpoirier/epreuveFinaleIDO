#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <stdbool.h>
#include <math.h>

#include <pigpio.h>

#define PORT_SEND_DATA 2291
#define DEST_IP "10.10.0.185"
#define MESSAGE0 "0"
#define MESSAGE1 "1"

#define PINBUTTON 17
//test : nc -lp 2291

//nc 10.10.21.5 6543
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

    for (int i=0;i<0x0F;i+=2) {
        i2cWriteByteData(handle, i, 0x00);
    }   
    while(1) {
        int datalen;
        datalen = read(socket_dist, buffer, BUFFER_SIZE);
        if (datalen == 0) {
            printf("Déconnexion\n");
            break;
        } else {
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
            int equipeInInt = atoi(equipe);

            int col = (8 - equipeInInt % 8) - 1;
            int row = equipeInInt / 8;
            int adr = row *2;
            int bits = 1 << col;
            int lit = i2cReadByteData(handle, adr);

            bool isProblematicData = false;


            printf("Equipe : %i\n",equipeInInt);
            printf("row : %i\n",row);
            printf("col : %i\n",col);


            if(strcmp(onOrOff,"0\n") == 0 || strcmp(onOrOff,"1\n") == 0) {
                if(strcmp(onOrOff,"0\n") == 0){
                    bits = bits | lit;
                } else {
                    bits = ~bits; // Flip: 0110 -> 1001
                    bits = bits & lit;
                }


                i2cWriteByteData(handle,adr,bits);
                
                

            } else {
                printf("On or off out of range\n");
                printf("Need to be 0 or 1\n");
            }

            fflush(stdout); 
        }
        memset(buffer, 0, BUFFER_SIZE); 
    }
    close(socket_dist);
    close(socket_local);
    i2cClose(handle);
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