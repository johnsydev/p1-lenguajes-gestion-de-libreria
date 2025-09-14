#include "datos.h"
#include "menu.h"
#include "auxiliares.h"
#include "libro.h"
#include "cliente.h"
#include "pedido.h"
#include "registro.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define input(texto, size) fflush(stdin); fgets(texto, size, stdin); texto[strcspn(texto, "\n")] = 0;

void menuRegistrarLibro() {
    int cant;
    struct Libro** libros = cargarLibros(&cant);
    char *codigo = malloc(20 * sizeof(char));
    char *nombre = malloc(100 * sizeof(char));
    char *autor  = malloc(50 * sizeof(char));
    float precio;
    int cantidad;

    printf("------- Agregar Libro -------\n\n");
    printf("\nCodigo del libro:");
    input(codigo, 20);
    printf("\nNombre del libro: ");
    input(nombre, 100);
    printf("\nAutor: ");
    input(autor, 50);
    printf("\nPrecio: ");
    scanf("%f", &precio);
    printf("\nCantidad disponible: ");
    scanf("%d", &cantidad);
    printf("\n\n");

    struct Libro* nuevoLibro = malloc(sizeof(struct Libro));

    nuevoLibro->codigo = asignarString(codigo);
    nuevoLibro->nombre = asignarString(nombre);
    nuevoLibro->autor  = asignarString(autor);

    nuevoLibro->precio = precio;
    nuevoLibro->cantidad = cantidad;

    if (registrarLibro(libros, nuevoLibro, cant)) {
        printf("Libro agregado correctamente.\n\n");
    }
    free(codigo);
    free(nombre);
    free(autor);
}

void menuRegistrarCliente() {
    int cant;
    struct Cliente** clientes = cargarClientes(&cant);
    char *cedula = malloc(TAM_CEDULA * sizeof(char));
    char *nombre = malloc(TAM_NOMBRE * sizeof(char));
    char *telefono = malloc(TAM_TELEFONO * sizeof(char));

    printf("------- Registrar Cliente -------\n\n");
    printf("Cedula: ");
    input(cedula, TAM_CEDULA);
    if (strlen(cedula) == 0) { 
        printf("Cedula no puede estar vacia.\n\n"); 
        return; 
    }

    printf("Nombre: ");
    input(nombre, TAM_NOMBRE);
    if (strlen(nombre) == 0) { 
        printf("Nombre no puede estar vacio.\n\n"); 
        return; 
    }

    printf("Telefono: ");
    input(telefono, TAM_TELEFONO);
    if (!validarTelefono(telefono)) { 
        printf("Telefono invalido. Debe tener al menos 7 digitos y solo numeros.\n\n"); 
        return; 
    }

    struct Cliente* nuevoCliente = malloc(sizeof(struct Cliente));

    nuevoCliente->cedula = asignarString(cedula);
    nuevoCliente->nombre = asignarString(nombre);
    nuevoCliente->telefono = asignarString(telefono);

    if (registrarCliente(clientes, nuevoCliente, cant)) {
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
    char *codigo = malloc(20 * sizeof(char));
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
                input(codigo, 20);
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

                char *cedula = malloc(TAM_CEDULA * sizeof(char));
                printf("Cédula del cliente: ");
                input(cedula, TAM_CEDULA);
                
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
                    free(generarIdPedido());
                    return;
                }
                break;
                
            case 5:
                printf("Saliendo sin guardar...\n\n");
                free(generarIdPedido());
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

    char *archivo = malloc(100 * sizeof(char));
    input(archivo, 100);
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
        printf("1. Total de ventas por anio\n");
        printf("2. Total de ventas por mes/anio\n");
        printf("3. Clientes con mas pedidos\n");
        printf("4. Libros mas vendidos\n");
        printf("5. Autor con mas ventas por anio\n");
        printf("6. Volver\n\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Opcion no valida.\n\n");
            continue;
        }
        { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

        if (opcion == 1) {
            mostrarTotalVentasPorAnio(pedidos, cantidadPedidos);
            printf("Presione ENTER para continuar..."); getchar(); printf("\n");
        }
        else if (opcion == 2) {
            mostrarVentasPorMesAnio(pedidos, cantidadPedidos);
            printf("Presione ENTER para continuar..."); getchar(); printf("\n");
        }
        else if (opcion == 3) {
            int topN = 0;
            printf("Cuantos desea ver (Top N): ");
            if (scanf("%d", &topN) != 1) { topN = 5; }
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            if (topN <= 0) topN = 5;

            mostrarClientesConMasPedidos(pedidos, cantidadPedidos, topN);
            printf("Presione ENTER para continuar..."); getchar(); printf("\n");
        }
        else if (opcion == 4) {
            int topN = 0;
            char *anio = malloc(8 * sizeof(char));
            anio[0] = '\0';

            printf("Cuantos desea ver (Top N): ");
            if (scanf("%d", &topN) != 1) { topN = 5; }
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            if (topN <= 0) topN = 5;

            printf("Filtrar por anio (ej: 2025) o ENTER para todos: ");
            input(anio, sizeof(anio));
            if (anio[0] == '\0') {
                mostrarLibrosMasVendidos(pedidos, cantidadPedidos, NULL, topN);
            } else {
                mostrarLibrosMasVendidos(pedidos, cantidadPedidos, anio, topN);
            }
            printf("Presione ENTER para continuar..."); getchar(); printf("\n");
        }
        else if (opcion == 5) {
            mostrarAutorTopPorAnio(pedidos, cantidadPedidos);
            printf("Presione ENTER para continuar..."); getchar(); printf("\n");
        }
        else if (opcion == 6) {
            printf("Volviendo...\n\n");
            break;
        }
        else {
            printf("Opcion no valida.\n\n");
        }
    }
}

void menuConsultaPedidos() {
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
        // Listado
        printf("------- Consulta de pedidos -------\n\n");
        printf("%-4s %-12s %-25s %-12s %-12s %-12s\n",
               "No.", "Identificador", "Cliente", "Fecha", "Subtotal", "Total");
        printf("-------------------------------------------------------------------------------\n");

        for (int i = 0; i < cantidadPedidos; i++) {
            struct Pedido* p = pedidos[i];
            printf("%-4d %-12s %-25s %-12s $%-11.2f $%-11.2f\n",
                   i + 1,
                   p->idPedido,
                   p->nombreCliente,
                   p->fecha,
                   p->subtotalPedido,
                   p->totalPedido);
        }
        printf("\n");

        int opcion = -1;
        printf("Seleccione un pedido por numero (1-%d) o 0 para volver: ", cantidadPedidos);
        if (scanf("%d", &opcion) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Opcion no valida.\n\n");
            continue;
        }
        // limpiar '\n'
        { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

        if (opcion == 0) {
            printf("Volviendo...\n\n");
            break;
        }
        if (opcion < 1 || opcion > cantidadPedidos) {
            printf("Numero fuera de rango.\n\n");
            continue;
        }

        struct Pedido* seleccionado = pedidos[opcion - 1];
        mostrarPedidoCompleto(seleccionado);

        printf("Presione ENTER para volver a la lista...");
        getchar();
        printf("\n");
    }
}

void menuEliminarLibro() {
    int cantLibros = 0, cantPedidos = 0;
    struct Libro** libros = cargarLibros(&cantLibros);
    struct Pedido** pedidos = cargarPedidos(&cantPedidos);

    if (cantLibros == 0) {
        printf("No hay libros para eliminar.\n\n");
        return;
    }

    char *codigo = malloc(20 * sizeof(char));
    { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

    printf("------- Eliminar Libro -------\n\n");
    printf("Codigo del libro: ");
    input(codigo, 20);

    if (strlen(codigo) == 0) {
        printf("El codigo no puede estar vacio.\n\n");
        return;
    }

    if (!eliminarLibro(&libros, &cantLibros, pedidos, cantPedidos, codigo)) {
        return;
    }
}

void menuEliminarCliente() {
    int cantClientes = 0, cantPedidos = 0;
    struct Cliente** clientes = cargarClientes(&cantClientes);
    struct Pedido** pedidos   = cargarPedidos(&cantPedidos);

    if (cantClientes == 0) {
        printf("No hay clientes para eliminar.\n\n");
        return;
    }

    char *cedula = malloc(TAM_CEDULA * sizeof(char));
    { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

    printf("------- Eliminar Cliente -------\n\n");
    printf("Cedula del cliente: ");
    input(cedula, TAM_CEDULA);

    if (strlen(cedula) == 0) {
        printf("La cedula no puede estar vacia.\n\n");
        return;
    }

    if (!eliminarCliente(&clientes, &cantClientes, pedidos, cantPedidos, cedula)) {
        return;
    }
}

bool menuLogin() {
    char *usuario = malloc(30 * sizeof(char));
    char *contrasena = malloc(30 * sizeof(char));
    printf("------- Autenticacion -------\n\n");
    printf("Nombre de usuario: ");
    input(usuario, 30);
    printf("\nContraseña: ");
    input(contrasena, 30);
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
    printf("2. Eliminar Libro\n");
    printf("3. Manejo de inventario\n\n");
    printf("4. Registrar clientes\n");
    printf("5. Eliminar Cliente\n\n");
    printf("6. Crear pedido\n");
    printf("7. Modificar Pedido\n");
    printf("8. Eliminar Pedido\n");
    printf("9. Consulta de pedidos\n\n");
    printf("10. Estadisticas\n");
    printf("11. Salir\n\n");

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
            menuEliminarLibro();
            break;
        case 3:
            menuManejoInventario();
            break;
        case 4:
            menuRegistrarCliente();
            break;
        case 5:
            menuEliminarCliente();
            break;
        case 6:
            menuCrearPedido();
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            menuConsultaPedidos();
            break;
        case 10:
            menuEstadisticas();
            break;
        case 11:
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

void menuConsultaCliente() {
    int cantidadClientes = 0, cantidadPedidos = 0;
    struct Cliente** clientes = cargarClientes(&cantidadClientes);
    struct Pedido** pedidos   = cargarPedidos(&cantidadPedidos);

    char *cedula = malloc(TAM_CEDULA * sizeof(char));

    { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
    printf("------- Consulta de cliente -------\n\n");
    printf("Cedula del cliente: ");
    input(cedula, TAM_CEDULA);
    printf("\n");

    if (strlen(cedula) == 0) {
        printf("La cedula no puede estar vacía.\n\n");
        return;
    }

    // Buscar cliente
    struct Cliente* cliente = buscarClientePorCedula(clientes, cantidadClientes, cedula);
    if (cliente == NULL) {
        printf("Cliente no encontrado.\n\n");
        return;
    }

    // Mostrar datos del cliente
    printf("=== DATOS DEL CLIENTE ===\n");
    printf("Cedula : %s\n", cliente->cedula);
    printf("Nombre : %s\n", cliente->nombre);
    printf("Telefono: %s\n\n", cliente->telefono);

    printf("=== PEDIDOS DEL CLIENTE ===\n");
    printf("%-10s %-12s %-10s %-10s %-15s\n",
           "Codigo", "Fecha", "Subtotal", "Total", "Cant. libros");
    printf("-------------------------------------------------------------\n");

   // Mostrar pedidos del cliente
    int pedidosMostrados = 0;
    for (int i = 0; i < cantidadPedidos; i++) {
        if (compararString(pedidos[i]->cedulaCliente, cedula)) {
            int cantidadLibros = 0;
            for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                cantidadLibros += pedidos[i]->detalles[j]->cantidad;
            }

            printf("%-10s %-12s $%-9.2f $%-9.2f %-15d\n",
                   pedidos[i]->idPedido,
                   pedidos[i]->fecha,
                   pedidos[i]->subtotalPedido,
                   pedidos[i]->totalPedido,
                   cantidadLibros);
            pedidosMostrados++;
        }
    }

    if (pedidosMostrados == 0) {
        printf("El cliente no tiene pedidos registrados.\n");
    }
    printf("\n");
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
            menuConsultaCliente();
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


