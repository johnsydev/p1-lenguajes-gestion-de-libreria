#include "datos.h"
#include "menu.h"
#include "auxiliares.h"
#include "libro.h"
#include "cliente.h"
#include "pedido.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

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
    printf("\nCodigo del libro:");
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

void menuRegistrarCliente() {
    int cant;
    struct Cliente** clientes = cargarClientes(&cant);
    char cedula[TAM_CEDULA];
    char nombre[TAM_NOMBRE];
    char telefono[TAM_TELEFONO];

    printf("------- Registrar Cliente -------\n\n");
    printf("Cedula: ");
    input(cedula);
    if (strlen(cedula) == 0) { 
        printf("Cedula no puede estar vacia.\n\n"); 
        return; 
    }

    printf("Nombre: ");
    input(nombre);
    if (strlen(nombre) == 0) { 
        printf("Nombre no puede estar vacio.\n\n"); 
        return; 
    }

    printf("Teléfono: ");
    input(telefono);
    if (!validarTelefono(telefono)) { 
        printf("Telefono invalido. Debe tener al menos 7 digitos y solo numeros.\n\n"); 
        return; 
    }

    struct Cliente nuevoCliente;
    copiarString(nuevoCliente.cedula, cedula);
    copiarString(nuevoCliente.nombre, nombre);
    copiarString(nuevoCliente.telefono, telefono);

    if (registrarCliente(clientes, &nuevoCliente, cant)) {
        printf("Cliente registrado correctamente.\n\n");
    }
}

void menuCrearPedido() {
    int cantLibros, cantClientes;
    struct Libro** libros = cargarLibros(&cantLibros);
    struct Cliente** clientes = cargarClientes(&cantClientes);
    
    if (cantLibros == 0) {
        printf("No hay libros disponibles para crear pedidos.\n\n");
        return;
    }
    
    struct Pedido pedido;
    pedido.detalles = NULL;
    pedido.cantidadDetalles = 0;
    
    printf("------- Crear Pedido -------\n\n");
    
    int opcion;
    char codigo[20];
    int cantidad;
    int numeroLinea;
    
    do {
        printf("=== GESTION DE PEDIDO ===\n");
        printf("1. Agregar linea\n");
        printf("2. Eliminar linea\n");
        printf("3. Mostrar pedido actual\n");
        printf("4. Generar pedido\n");
        printf("5. Salir sin guardar\n\n");
        
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        printf("\n");
        
        switch (opcion) {
            case 1:
                printf("Codigo del libro: ");
                input(codigo);
                printf("Cantidad: ");
                scanf("%d", &cantidad);
                
                if (cantidad <= 0) {
                    printf("La cantidad debe ser mayor a 0.\n\n");
                    break;
                }
                
                if (agregarDetallePedido(&pedido.detalles, &pedido.cantidadDetalles, codigo, cantidad, libros, cantLibros)) {
                    printf("Linea agregada correctamente.\n\n");
                }
                break;
                
            case 2:
                if (pedido.cantidadDetalles == 0) {
                    printf("No hay lineas para eliminar.\n\n");
                    break;
                }
                
                mostrarDetallePedido(pedido.detalles, pedido.cantidadDetalles);
                printf("Numero de línea a eliminar: ");
                scanf("%d", &numeroLinea);
                eliminarDetallePedido(&pedido.detalles, &pedido.cantidadDetalles, numeroLinea);
                break;
                
            case 3:
                mostrarDetallePedido(pedido.detalles, pedido.cantidadDetalles);
                if (pedido.cantidadDetalles > 0) {
                    float subtotal;
                    float impuesto;
                    float total;
                    calcularTotalesPedido(pedido.detalles, pedido.cantidadDetalles, &subtotal, &impuesto, &total);
                    printf("Subtotal: $%.2f\n", subtotal);
                    printf("Impuesto (13%%): $%.2f\n", impuesto);
                    printf("Total: $%.2f\n\n", total);
                }
                break;
                
            case 4:
                if (pedido.cantidadDetalles == 0) {
                    printf("No se puede generar un pedido vacio.\n\n");
                    break;
                }
                
                char cedula[15];
                printf("Cédula del cliente: ");
                input(cedula);
                
                struct Cliente* cliente = buscarClientePorCedula(clientes, cantClientes, cedula);
                if (cliente == NULL) {
                    printf("Cliente no encontrado.\n\n");
                    break;
                }
                
                // Llenar datos del pedido
                copiarString(pedido.idPedido, generarIdPedido());
                copiarString(pedido.cedulaCliente, cliente->cedula);
                copiarString(pedido.nombreCliente, cliente->nombre);
                
                // Fecha actual
                time_t t = time(NULL);
                struct tm *fecha = localtime(&t);
                snprintf(pedido.fecha, TAM_FECHA, "%02d/%02d/%04d", 
                        fecha->tm_mday, fecha->tm_mon + 1, fecha->tm_year + 1900);
                
                calcularTotalesPedido(pedido.detalles, pedido.cantidadDetalles, 
                                     &pedido.subtotalPedido, &pedido.impuesto, &pedido.totalPedido);
                
                if (generarPedido(&pedido, libros, &cantLibros)) {
                    mostrarPedidoCompleto(&pedido);
                    printf("Pedido generado exitosamente.\n\n");
                    return;
                }
                break;
                
            case 5:
                printf("Saliendo sin guardar...\n\n");
                break;
                
            default:
                printf("Opcion no válida.\n\n");
                break;
        }
    } while (opcion != 4 && opcion != 5);
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

void menuEstadisticas(void) {
    int cantidadPedidos = 0;
    struct Pedido** pedidos = cargarPedidos(&cantidadPedidos);

    if (pedidos == NULL || cantidadPedidos == 0) {
        printf("No hay pedidos registrados.\n\n");
        printf("Presione ENTER para volver...");
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
        getchar();  
        return;
    }

    while (1) {
        int opcion = 0;

        printf("------- Estadisticas -------\n\n");
        printf("1. Total de ventas\n");
        printf("2. Clientes con mas pedidos\n");
        printf("3. Libros mas vendidos\n");
        printf("4. Volver\n\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            // entrada inválida 
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Opcion no valida.\n\n");
            continue;
        }
        // limpia '\n' pendiente
        { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

        if (opcion == 1) {
            mostrarTotalVentasPorAnio(pedidos, cantidadPedidos);
            printf("Presione ENTER para continuar...");
            getchar();  
            printf("\n");
        }
        else if (opcion == 2) {
            int topN = 0;
            printf("Cuantos desea ver (Top N): ");
            if (scanf("%d", &topN) != 1) { topN = 5; }
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            if (topN <= 0) topN = 5;

            printf("\n");
            mostrarClientesConMasPedidos(pedidos, cantidadPedidos, topN);
            printf("Presione ENTER para continuar...");
            getchar();
            printf("\n");
        }
        else if (opcion == 3) {
            int topN = 0;
            char anio[8]; anio[0] = '\0';

            printf("Cuantos desea ver (Top N): ");
            if (scanf("%d", &topN) != 1) { topN = 5; }
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            if (topN <= 0) topN = 5;

            printf("Filtrar por año (ej: 2025) o ENTER para todos: ");
            input(anio);  

            printf("\n");
            if (anio[0] == '\0') {
                mostrarLibrosMasVendidos(pedidos, cantidadPedidos, NULL, topN);
            } else {
                mostrarLibrosMasVendidos(pedidos, cantidadPedidos, anio, topN);
            }
            printf("Presione ENTER para continuar...");
            getchar();
            printf("\n");
        }
        else if (opcion == 4) {
            printf("Volviendo...\n\n");
            break;
        }
        else {
            printf("Opcion no valida.\n\n");
        }
    }
}

bool menuLogin() {
    char usuario[30];
    char contrasena[30];
    printf("------- Autenticacion -------\n\n");
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
    printf("5. Estadisticas\n");
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
            menuRegistrarCliente();
            break;
        case 4:
            menuCrearPedido();
            break;
        case 5:
            menuEstadisticas();
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
    printf("1. Consulta de catalogo\n");
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


