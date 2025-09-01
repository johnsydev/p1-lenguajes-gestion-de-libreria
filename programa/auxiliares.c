#include "auxiliares.h"
#include <stdbool.h>

void copiarString(char destino[], char origen[]) {
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

bool compararString(char str1[], char str2[]) {
    int i = 0;
    while (str1[i] != '\0')
    {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    if (str1[i] != str2[i]) return false;
    return true;
}