#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"

bool menuLogin(struct Configuracion* config);
void menuEstadisticas();
void menuPrincipal();
void menuAdministrativo(struct Configuracion* config);
void menuOpcionesPrincipales(); 

void menuRegistrarLibro();
void menuEliminarLibro();
void menuRegistrarCliente();
void menuEliminarCliente();
void menuCrearPedido(struct Configuracion* config);
void menuEliminarPedido();
void menuModificarPedido(struct Configuracion* config);

void menuMostrarCatalogo();
void menuConsultaCliente();
void menuConsultaPedidos(struct Configuracion* config);
void menuManejoInventario();

#endif