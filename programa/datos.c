#include "datos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
    Nombre: leerArchivo
    Entrada: Nombre del archivo (char*) y cantidad de lineas (int*)
    Salida: Lineas del archivo (char**)
    Objetivo: 
        Leer el contenido de un archivo linea por linea y agregarlo a un arreglo de lineas
        Para esto, se usa fgets para recorrer linea a linea, realloc para ir redimensionando el arreglo de
        lineas, malloc para asignar memoria en el espacio del arreglo y strcpy para copiar la linea leida.
*/
char** leerArchivo(char* nombreArchivo, int* cantidadLineas) {
    char** lineas = NULL;
    int cantidad = 0;
    char linea[150];
    FILE *archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return NULL;
    }

    while(fgets(linea, 150, archivo)) {
        linea[strcspn(linea, "\n")] = 0;
        cantidad++;
        lineas = (char**)realloc(lineas, cantidad * sizeof(char*));
        lineas[cantidad-1] = (char*)malloc(strlen(linea) + 1);
        strcpy(lineas[cantidad-1], linea);
    }

    fclose(archivo);
    *cantidadLineas = cantidad;
    return lineas;
}


bool verificarAdmin(char* usuario, char* contrasena) {
    int* cantidadLineas;
    char** credenciales = leerArchivo("acceso.txt", cantidadLineas);

    if (credenciales == NULL) {
        printf("Error al leer el archivo.\n");
        return false;
    }
    
    for (int i = 0; i < *cantidadLineas; i++) {
        char** info = separarTexto(credenciales[i], ';', 2);
        char* infoUsuario = info[0];
        char* infoContrasena = info[1];
        if (strcmp(infoUsuario, usuario) == 0 && strcmp(infoContrasena, contrasena) == 0) {
            return true;
        }
    }
    return false;
}

/*
    Nombre: separarTexto
    Entrada: Texto (char*) y delimitador (char) y cantidad de separaciones (int)
    Salida: Texto separado (char**)
    Objetivo:
        Separar el texto por el delimitador y almacenarlo en un arreglo de cadenas de caracteres
        Para esto, se crea un arreglo de la cantidad de separaciones, se copia el texto en una variable temporal,
        se copia el texto en el arreglo de cadenas de caracteres y se retorna el arreglo despues de procesar todo.
        Como el delimitador nunca está al final de la cadena, se utiliza un while para recorrer la cadena y
        el último extracto de texto se agrega fuera del while.
*/
char** separarTexto(char* texto, char delimitador, int cantidad) {
    char** array = malloc(cantidad * sizeof(char*));
    char temp[100];
    int indiceArray = 0;
    int indiceTexto = 0;
    int indiceTemp = 0;
    while (texto[indiceTexto] != '\0') {
        if (texto[indiceTexto] == ';') {
            indiceTexto++;
            temp[indiceTemp] = '\0';
            array[indiceArray] = (char*)malloc(strlen(temp) + 1);
            strcpy(array[indiceArray], temp);
            indiceArray++;
            temp[0] = '\0';
            indiceTemp = 0;
            continue;
        }
        temp[indiceTemp] = texto[indiceTexto]; 
        indiceTexto++;
        indiceTemp++;
    }
    temp[indiceTemp] = '\0';
    array[indiceArray] = (char*)malloc(strlen(temp) + 1);
    strcpy(array[indiceArray], temp);
    return array;
};