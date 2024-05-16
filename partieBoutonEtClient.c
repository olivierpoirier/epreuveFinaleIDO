#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdbool.h>

#include <pigpio.h>

#define PORT 3581
#define DEST_IP "10.10.0.185"
#define MESSAGE0 "0"
#define MESSAGE1 "1"

#define PINBUTTON 17
//test : nc -lp 2291

int main() {
    int sock = 0;
    struct sockaddr_in dest_addr;
    
    // Créer le socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Initialiser la struct de l'adresse IP 
    memset(&dest_addr, '0', sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    dest_addr.sin_port = htons(PORT);

    if (gpioInitialise() < 0) {
        fprintf(stderr, "Erreur d'initialisation pigpio\n");
        return 1;
    }
    gpioSetMode(PINBUTTON, PI_INPUT);

    // Créer la connexion
    int con = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    printf("Ready To send data!\n");
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
    close(sock);
    gpioTerminate();
    return 0;
}