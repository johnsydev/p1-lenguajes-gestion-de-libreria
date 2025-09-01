#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool menuLogin();
void menuPrincipal();
void menuAdministrativo();
void menuOpcionesPrincipales(); //sin autenticación

void menuRegistrarLibro();

void menuMostrarCatalogo();
void menuManejoInventario();

void copiarString();

#endif