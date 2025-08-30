#include "menu.h"
#include <string.h>
#include <stdbool.h>

bool menuLogin() {
    char usuario[30];
    char contrasena[30];
    printf("------- Autenticación -------\n\n");
    printf("Nombre de usuario: ");
    scanf("%s", usuario);
    printf("\nContraseña: ");
    scanf("%s", contrasena);
    printf("\n\n");

    if (strcmp(usuario, "admin") == 0 && strcmp(contrasena, "admin") == 0) {
        printf("Acceso concedido.\n\n");
        return true;
    }
    printf("Usuario o contraseña incorrectos.\n\n");
    return false;    
}

void menuAdministrativo() {
    printf("------- Menu Administrativo -------\n\n");
    printf("1. Registrar libros\n");
    printf("2. Manejo de inventario\n");
    printf("3. Registrar clientes\n");
    printf("4. Crear pedido\n");
    printf("5. Estadísticas\n");
    printf("6. Salir\n\n");

    int opcion;
    printf("Seleccione una opcion: ");
    scanf("%d", &opcion);
    printf("\n\n");

    switch (opcion)
    {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            menuPrincipal();
            break;
        default:
            printf("Opcion no valida.\n\n");
            break;
    }
    menuAdministrativo();
}

void menuOpcionesPrincipales() {
    printf("------- Opciones Principales -------\n\n");
    printf("1. Consulta de catálogo\n");
    printf("2. Consulta de cliente\n");
    printf("3. Salir\n\n");

    int opcion;
    printf("Seleccione una opcion: ");
    scanf("%d", &opcion);
    printf("\n\n");

    switch (opcion)
    {
        case 1:
            break;
        case 2:
            break;
        case 3:
            menuPrincipal();
            break;
        default:
            printf("Opcion no valida.\n\n");
            break;
    }
    menuOpcionesPrincipales();
}

void menuPrincipal() {
    printf("------- Menu Principal -------\n\n");
    printf("1. Administrativo\n");
    printf("2. Opciones principales\n");
    printf("3. Salir\n\n");

    int opcion;
    bool auth = false;
    printf("Seleccione una opcion: ");
    scanf("%d", &opcion);
    printf("\n\n");

    switch (opcion)
    {
        case 1:
            auth = menuLogin();
            if (auth) {
                menuAdministrativo();
            }
            break;
        case 2:
            menuOpcionesPrincipales();
            break;
        case 3:
            exit(0);
            break;
        default:
            printf("Opcion no valida.\n\n");
            break;
    }
    menuPrincipal();
}