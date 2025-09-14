#ifndef CLIENTE_H
#define CLIENTE_H

#define TAM_CEDULA 15
#define TAM_NOMBRE 50
#define TAM_TELEFONO 15

struct Cliente {
    char *cedula;
    char *nombre;
    char *telefono;
};

#endif