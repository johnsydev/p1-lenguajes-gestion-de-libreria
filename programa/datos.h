#ifndef DATOS_H
#define DATOS_H

#include <stdbool.h>
#include "libro.h"

char** leerArchivo(char*, int*);

bool verificarAdmin(char*, char*);
char** separarTexto(char*, char, int);

bool registrarLibro(struct Libro**, struct Libro*, int);
struct Libro** cargarLibros();
void actualizarTodosLibros(struct Libro**, int*);

bool modificarInventario(struct Libro**, int*, char*);
#endif