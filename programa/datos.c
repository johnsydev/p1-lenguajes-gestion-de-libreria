#include "cliente.h"
// CLIENTES
struct Cliente** cargarClientes(int* cant) {
    int cantidadLineas;
    char** clientesTxt = leerArchivo(CLIENTES_TXT, &cantidadLineas);
    struct Cliente** clientes = malloc(cantidadLineas * sizeof(struct Cliente*));
    if (clientesTxt == NULL) {
        *cant = 0;
        return NULL;
    }
    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(clientesTxt[i], ';', 3);
        struct Cliente* cliente = malloc(sizeof(struct Cliente));
        copiarString(cliente->cedula, info[0]);
        copiarString(cliente->nombre, info[1]);
        copiarString(cliente->telefono, info[2]);
        clientes[i] = cliente;
        for (int j = 0; j < 3; j++) free(info[j]);
        free(info);
    }
    *cant = cantidadLineas;
    return clientes;
}

int existeCedula(struct Cliente** clientes, int cantidad, char* cedula) {
    for (int i = 0; i < cantidad; i++) {
        if (compararString(clientes[i]->cedula, cedula)) {
            return 1;
        }
    }
    return 0;
}

int validarTelefono(char* telefono) {
    int len = strlen(telefono);
    if (len < 7) return 0;
    for (int i = 0; i < len; i++) {
        if (telefono[i] < '0' || telefono[i] > '9') {
            return 0;
        }
    }
    return 1;
}

bool registrarCliente(struct Cliente** listaClientes, struct Cliente* cliente, int cantidadClientes) {
    for (int i = 0; i < cantidadClientes; i++) {
        if (compararString(listaClientes[i]->cedula, cliente->cedula)) {
            printf("Error: Ya existe un cliente con esa cédula.\n\n");
            return false;
        }
    }

    listaClientes = (struct Cliente**)realloc(listaClientes, (cantidadClientes + 1) * sizeof(struct Cliente*));
    if (listaClientes == NULL) {
        return false;
    }
    listaClientes[cantidadClientes] = cliente;
    cantidadClientes++;

    FILE *archivo = fopen(CLIENTES_TXT, "a");
    if (cantidadClientes == 1) {
        fprintf(archivo, "%s;%s;%s", cliente->cedula, cliente->nombre, cliente->telefono);
    } else {
        fprintf(archivo, "\n%s;%s;%s", cliente->cedula, cliente->nombre, cliente->telefono);
    }
    fclose(archivo);
    return true;
}

void actualizarTodosClientes(struct Cliente** clientes, int* cantidadClientes) {
    FILE *archivo = fopen(CLIENTES_TXT, "w");
    for (int i = 0; i < *cantidadClientes; i++) {
        if (i == 0) {
            fprintf(archivo, "%s;%s;%s", clientes[i]->cedula, clientes[i]->nombre, clientes[i]->telefono);
        } else {
            fprintf(archivo, "\n%s;%s;%s", clientes[i]->cedula, clientes[i]->nombre, clientes[i]->telefono);
        }
    }
    fclose(archivo);
}
#include "datos.h"
#include "libro.h"
#include "auxiliares.h"
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
        *cantidadLineas = 0;
        printf("Error al abrir el archivo.\n\n");
        return NULL;
    }

    while(fgets(linea, 150, archivo)) {
        linea[strcspn(linea, "\n")] = 0;
        cantidad++;
        lineas = (char**)realloc(lineas, cantidad * sizeof(char*));
        lineas[cantidad-1] = (char*)malloc(strlen(linea) + 1);
        copiarString(lineas[cantidad-1], linea);
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
        if (compararString(infoUsuario, usuario) == true && compararString(infoContrasena, contrasena) == true) {
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
            copiarString(array[indiceArray], temp);
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
    copiarString(array[indiceArray], temp);
    return array;
};


/* GESTION DE LIBROS */

struct Libro** cargarLibros(int* cant) {
    int cantidadLineas;
    char** librosTxt = leerArchivo("libros.txt", &cantidadLineas);
    struct Libro** libros = malloc(cantidadLineas * sizeof(struct Libro*));

    if (librosTxt == NULL) {
        printf("Error al leer el archivo.\n");
        return NULL;
    }
    
    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(librosTxt[i], ';', 5);
        struct Libro* libro = malloc(sizeof(struct Libro));
        copiarString(libro->codigo, info[0]);
        copiarString(libro->nombre, info[1]);
        copiarString(libro->autor, info[2]);
        libro->precio = atof(info[3]);
        libro->cantidad = atoi(info[4]);
        libros[i] = libro;

        for (int j = 0; j < 5; j++) free(info[j]);
        free(info);
    }
    *cant = cantidadLineas;
    return libros;
}


bool registrarLibro(struct Libro** listaLibros, struct Libro* libro, int cantidadLibros) {
    for (int i = 0; i < cantidadLibros; i++) {
        if (strcmp(listaLibros[i]->codigo, libro->codigo) == 0) {
            printf("Error: Ya existe un libro con ese codigo.\n\n");
            return false;
        }
    }

    listaLibros = (struct Libro**)realloc(listaLibros, (cantidadLibros + 1) * sizeof(struct Libro*));
    if (listaLibros == NULL) {
        return false;
    }
    listaLibros[cantidadLibros] = libro;
    cantidadLibros++;

    FILE *archivo = fopen("libros.txt", "a");
    if (cantidadLibros == 1) {
        fprintf(archivo, "%s;%s;%s;%.2f;%d", libro->codigo, libro->nombre, libro->autor, libro->precio, libro->cantidad);
    }
    fprintf(archivo, "\n%s;%s;%s;%.2f;%d", libro->codigo, libro->nombre, libro->autor, libro->precio, libro->cantidad);
    fclose(archivo);

    return true;
}

void actualizarTodosLibros(struct Libro** libros, int* cantidadLibros) {
    FILE *limpiar = fopen("libros.txt", "w");
    fclose(limpiar);

    FILE *archivo = fopen("libros.txt", "a");

    for (int i = 0; i < *cantidadLibros; i++)
    {
        if (i == 0) {
            fprintf(archivo, "%s;%s;%s;%.2f;%d", libros[i]->codigo, libros[i]->nombre, libros[i]->autor, libros[i]->precio, libros[i]->cantidad);
        }
        else {
            fprintf(archivo, "\n%s;%s;%s;%.2f;%d", libros[i]->codigo, libros[i]->nombre, libros[i]->autor, libros[i]->precio, libros[i]->cantidad);
        }
    }  
    fclose(archivo);
}

bool modificarInventario(struct Libro** libros, int* cantidadLibros, char* archivo) {
    int cantidadLineas;
    char** contenido = leerArchivo(archivo, &cantidadLineas);
    if (contenido == NULL) return false;

    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(contenido[i], ';', 2);
        for (int j = 0; j < *cantidadLibros; j++) {
            if (compararString(libros[j]->codigo, info[0]) == true) {
                int count = libros[j]->cantidad + atoi(info[1]);
                if (count > 0) {
                    libros[j]->cantidad = count;
                }
                else {
                    printf("Linea %d no procesada, el stock quedaria negativo.\n", i+1);
                }
            }
        }
    }
    printf("\n");
    actualizarTodosLibros(libros, cantidadLibros);
    return true;
}