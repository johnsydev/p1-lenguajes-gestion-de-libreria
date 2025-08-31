#ifndef LIBRO_H
#define LIBRO_H

struct Libro {
    char codigo[20];
    char nombre[100];
    char autor[50];
    float precio;
    int cantidad;
};

#endif
