#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool menuLogin();
void menuEstadisticas();
void menuPrincipal();
void menuAdministrativo();
void menuOpcionesPrincipales(); //sin autenticación

void menuRegistrarLibro();
void menuRegistrarCliente();
void menuCrearPedido();

void menuMostrarCatalogo();
void menuConsultaCliente();
void menuConsultaPedidos();
void menuManejoInventario();

#endif