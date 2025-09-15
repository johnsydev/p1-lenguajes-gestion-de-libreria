#include "auxiliares.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
    Nombre: copiarString
    Entrada: destino (char*), origen (char*)
    Salida: void
    Objetivo:
        Copiar manualmente el contenido del string origen en el destino.
        No realiza asignación de memoria, asume que destino tiene
        suficiente espacio reservado.
*/
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
    Entrada: origen (char*)
    Salida: char* (puntero a nuevo string copiado en memoria dinámica)
    Objetivo:
        Reservar memoria dinámica suficiente para copiar el string origen,
        copiar carácter por carácter y devolver el puntero al nuevo string.
        El llamador debe liberar la memoria.
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

/*
    Nombre: compararString
    Entrada: str1 (char*), str2 (char*)
    Salida: bool (true si son iguales, false si difieren)
    Objetivo:
        Comparar carácter por carácter dos cadenas de texto terminadas en '\0'.
        Devuelve true solo si tienen exactamente el mismo contenido.
*/
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
