#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pigpio.h>

// Adresses 
#define ADR_I2C 0x70
#define ADR_SYS_MATRICE 0x20
#define ADR_AFFICHAGE_MATRICE 0x80

int HANDLE;

// Allumer / éteindre la nième LED. Convertir le nombre
// séquentiel (0 en premier) de LED en rangée / colonne.
// Les LED déjà allumées ne sont pas éteintes.
// ----------------
// OR pour allumer:
// 1000 - dejà allumé
// 0001 - à allumer
// 1001 - OR
// ----------------
// Inverser + AND pour éteindre
// 1010 - déjà allumé
// Cas 1:
// 1000 - à éteindre
// 0111 - Inverser
// 1010 & 0111 = 0010
// Cas 2 (quand on veut éteindre une LED déjà éteinte):
// 0100 - à éteindre
// 1011 - NEG
// 1010 & 1011 = 1010 (ne change pas)

int switch_led_seq(int num, int val) {

    int col = (8 - num % 8) - 1;
    int row = num / 8;
    int adr = row *2;
    int bits = 1 << col;
    int lit = i2cReadByteData(HANDLE,adr);

    if (val == 1) {
        bits = bits | lit;
    }
    else {
        bits = ~bits; // Flip: 0110 -> 1001
        bits = bits & lit;
    }

    i2cWriteByteData(HANDLE,adr,bits);

}

void all_off() {
    for (int i=0;i<=0x0e;i+=2) {
        i2cWriteByteData(HANDLE,i,0);
    }
}

void test_led() {
    switch_led_seq(0,1); // Allumer premier
    time_sleep(1);
    switch_led_seq(63,1); // Allumer dernier
    time_sleep(1);
    switch_led_seq(8,1); // 2e rangée, 1ere LED
    time_sleep(1);
    switch_led_seq(9,0); // Éteindre une LED déjà éteinte (rien ne se passe)
    time_sleep(1);
    switch_led_seq(9,1);
    time_sleep(1);
    switch_led_seq(7,1); // Première rangée, dernière LED
    time_sleep(1);
    switch_led_seq(9,0);
    time_sleep(1);
    switch_led_seq(7,0);
    time_sleep(1);
    switch_led_seq(8,0);
    time_sleep(1);
    switch_led_seq(0,0);
    time_sleep(1);
    switch_led_seq(63,0);
    time_sleep(1);
}

int main() {
    
    // Initialiser pigpio
    if (gpioInitialise() < 0) {
        printf("Erreur d'initialisation pigpio\n");
        return 1;
    }

    // Récupérer le référence ("handle") de la matrice
    HANDLE = i2cOpen(1, ADR_I2C, 0);
    
    if (HANDLE < 0) {
        printf("Erreur d'accès à la matrice LED\n");
        gpioTerminate();
        return 1;
    }

    all_off();
    test_led();
}

