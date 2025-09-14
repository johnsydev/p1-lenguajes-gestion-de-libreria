#include "auxiliares.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void copiarString(char* destino, char* origen) {
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

/*
    Nombre: asignarString
    Entrada: Texto origen (char*)
    Salida: Texto destino (char*)
    Objetivo:
        Esta función es un copiarString más sofisticado, que asigna la memoria, luego asigna el texto en el espacio y devuelve el puntero.
*/
char* asignarString(char* origen) {
    char* destino = malloc(strlen(origen) + 1);
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
    return destino;
}

bool compararString(char *str1, char *str2) {
    int i = 0;
    while (str1[i] != '\0')
    {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    if (str1[i] != str2[i]) return false;
    return true;
}