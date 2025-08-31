#ifndef DATOS_H
#define DATOS_H

#include <stdbool.h>

char** leerArchivo(char*, int*);

bool verificarAdmin(char*, char*);
char** separarTexto(char*, char, int);

//bool agregarLibro(struct**, struct*);
struct Libro** cargarLibros();
#endif