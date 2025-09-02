#ifndef DATOS_H
#define DATOS_H

#include <stdbool.h>

#include "libro.h"
#include "cliente.h"

#define CLIENTES_TXT "clientes.txt"

char** leerArchivo(char*, int*);
bool verificarAdmin(char*, char*);
char** separarTexto(char*, char, int);

// Libros
bool registrarLibro(struct Libro**, struct Libro*, int);
struct Libro** cargarLibros(int*);
void actualizarTodosLibros(struct Libro**, int*);
bool modificarInventario(struct Libro**, int*, char*);

// Clientes
struct Cliente** cargarClientes(int*);
bool registrarCliente(struct Cliente**, struct Cliente*, int);
void actualizarTodosClientes(struct Cliente**, int*);
int validarTelefono(char* telefono);
int existeCedula(struct Cliente**, int, char*);
#endif