#include "menu.h"
#include "datos.h"
#include "auxiliares.h"
#include "libro.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define input(texto) fflush(stdin); fgets(texto, sizeof(texto), stdin); texto[strcspn(texto, "\n")] = 0;

void menuRegistrarLibro() {
    int cant;
    struct Libro** libros = cargarLibros(&cant);
    char codigo[20];
    char nombre[100];
    char autor[50];
    float precio;
    int cantidad;

    printf("------- Agregar Libro -------\n\n");
    printf("\nCódigo del libro:");
    input(codigo);
    printf("\nNombre del libro: ");
    input(nombre);
    printf("\nAutor: ");
    input(autor);
    printf("\nPrecio: ");
    scanf("%f", &precio);
    printf("\nCantidad disponible: ");
    scanf("%d", &cantidad);
    printf("\n\n");

    struct Libro nuevoLibro;
    copiarString(nuevoLibro.codigo, codigo);
    copiarString(nuevoLibro.nombre, nombre);
    copiarString(nuevoLibro.autor, autor);
    nuevoLibro.precio = precio;
    nuevoLibro.cantidad = cantidad;
    if (registrarLibro(libros, &nuevoLibro, cant)) {
        printf("Libro agregado correctamente.\n\n");
        return;
    }
}

void menuManejoInventario() {
    printf("------- Manejo de inventario -------\n\n");
    printf("Este menu permite modificar la cantidad de libros disponibles, cargando la informacion desde un archivo.\n\n");
    printf("Escriba el nombre de archivo o presione [Enter] para volver atras.\n\n> ");

    char archivo[100];
    input(archivo);
    if (archivo[0] == '\0') {
        return;
    }

    int cant;
    struct Libro** libros = cargarLibros(&cant);
    modificarInventario(libros, &cant, archivo);
    return;
}

bool menuLogin() {
    char usuario[30];
    char contrasena[30];
    printf("------- Autenticación -------\n\n");
    printf("Nombre de usuario: ");
    scanf("%s", usuario);
    printf("\nContraseña: ");
    scanf("%s", contrasena);
    printf("\n\n");

    if (verificarAdmin(usuario, contrasena)) {
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
            menuRegistrarLibro();
            break;
        case 2:
            menuManejoInventario();
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


void menuMostrarCatalogo() {
    int cant;
    struct Libro** libros = cargarLibros(&cant);
    printf("------- Catalogo -------\n\n");
    if (cant == 0) {
        printf("No hay libros disponibles.\n\n");
        return;
    }
    for (int i = 0; i < cant; i++) {
        printf("------------------\n");
        printf("Codigo: %s\n", libros[i]->codigo);
        printf("Nombre: %s\n", libros[i]->nombre);
        printf("Autor: %s\n", libros[i]->autor);
        printf("Precio: %.2f\n", libros[i]->precio);
        printf("Cantidad: %d\n", libros[i]->cantidad);
        printf("------------------\n\n");
    }
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
            menuMostrarCatalogo();
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