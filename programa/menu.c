#include "datos.h"
#include "menu.h"
#include "auxiliares.h"
#include "libro.h"
#include "cliente.h"
#include "pedido.h"
#include "registro.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/*
    Macro: input
    Entrada: buffer de texto (char*), tamaño máximo (size_t)
    Salida: escribe en el buffer la línea leída sin el '\n' final
    Objetivo:
        Leer una línea de stdin, limpiar el '\n' y dejarla lista para uso.
        (Usa fflush(stdin) + fgets + eliminación del salto de línea).
*/
#define input(texto, size) fflush(stdin); fgets(texto, size, stdin); texto[strcspn(texto, "\n")] = 0;

/*
    Nombre: menuRegistrarLibro
    Entrada: ninguna
    Salida: void
    Objetivo:
        Solicitar datos del libro por consola, crear la estructura y
        llamar a registrarLibro para persistirlo. Maneja liberación de memoria.
*/
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
    if (libros) {
        for (int i = 0; i < cant; i++) {
            free(libros[i]->codigo);
            free(libros[i]->nombre);
            free(libros[i]->autor);
            free(libros[i]);
        }
        free(libros);
    }
    free(nuevoLibro->codigo);
    free(nuevoLibro->nombre);
    free(nuevoLibro->autor);
    free(nuevoLibro);
}

/*
    Nombre: menuRegistrarCliente
    Entrada: ninguna
    Salida: void
    Objetivo:
        Pedir datos del cliente, validar campos básicos y registrar el cliente.
        Libera toda la memoria utilizada independientemente del resultado.
*/
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
        free(cedula); free(nombre); free(telefono);
        if (clientes) {
            for (int i = 0; i < cant; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        return; 
    }

    printf("Nombre: ");
    input(nombre, TAM_NOMBRE);
    if (strlen(nombre) == 0) { 
        printf("Nombre no puede estar vacio.\n\n"); 
        free(cedula); free(nombre); free(telefono);
        if (clientes) {
            for (int i = 0; i < cant; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        return; 
    }

    printf("Telefono: ");
    input(telefono, TAM_TELEFONO);
    if (!validarTelefono(telefono)) { 
        printf("Telefono invalido. Debe tener al menos 7 digitos y solo numeros.\n\n"); 
        free(cedula); free(nombre); free(telefono);
        if (clientes) {
            for (int i = 0; i < cant; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        return; 
    }

    struct Cliente* nuevoCliente = malloc(sizeof(struct Cliente));

    nuevoCliente->cedula = asignarString(cedula);
    nuevoCliente->nombre = asignarString(nombre);
    nuevoCliente->telefono = asignarString(telefono);

    struct Cliente** temp = registrarCliente(clientes, nuevoCliente, &cant);
    if (temp != NULL) {
        if (temp != clientes) clientes = temp;  
        printf("Cliente registrado correctamente.\n\n");
    } else {
        printf("No se pudo registrar el cliente.\n\n");
    }
    free(cedula); 
    free(nombre); 
    free(telefono);
    if (clientes) {
        for (int i = 0; i < cant; i++) {
            free(clientes[i]->cedula);
            free(clientes[i]->nombre);
            free(clientes[i]->telefono);
            free(clientes[i]);
        }
        free(clientes);
    }
    free(nuevoCliente->cedula);
    free(nuevoCliente->nombre);
    free(nuevoCliente->telefono);
    free(nuevoCliente);
}

/*
    Nombre: menuCrearPedido
    Entrada: puntero a Configuracion
    Salida: void
    Objetivo:
        Interfaz para construir un pedido: agregar/eliminar líneas, mostrar
        totales y finalmente generar el pedido (persistir encabezado/detalles).
*/
void menuCrearPedido(struct Configuracion* config) {
    int cantLibros, cantClientes;
    struct Libro** libros = cargarLibros(&cantLibros);
    struct Cliente** clientes = cargarClientes(&cantClientes);
    
    if (cantLibros == 0) {
        printf("No hay libros disponibles para crear pedidos.\n\n");
        if (libros) free(libros);
        if (clientes) {
            for (int i = 0; i < cantClientes; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        return;
    }
    
    struct Pedido pedido = {0};
    pedido.detalles = NULL;
    pedido.cantidadDetalles = 0;
    pedido.cedulaCliente = NULL;
    pedido.nombreCliente = NULL;
    pedido.fecha = NULL;
    
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
            case 1: {
                
                char* filtroAutor = malloc(60 * sizeof(char));
                { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
                printf("Filtrar por autor (ENTER para todos): ");
                input(filtroAutor, 60);

                printf("\n--- Catalogo disponible ---\n");
                printf("%-6s %-25s %-20s %-8s\n", "No.", "Codigo", "Nombre", "Precio");
                printf("-------------------------------------------------------------\n");
                for (int i = 0, shown = 0; i < cantLibros; i++) {
                    if (filtroAutor[0] == '\0' || (libros[i]->autor && strstr(libros[i]->autor, filtroAutor) != NULL)) {
                        printf("%-6d %-25s %-20s $%-7.2f\n", ++shown, libros[i]->codigo, libros[i]->nombre, libros[i]->precio);
                    }
                }
                printf("\n");

                printf("Ingrese el codigo del libro a agregar (o ENTER para cancelar): ");
                input(codigo, 20);
                if (codigo[0] == '\0') {
                    printf("Operacion cancelada.\n\n");
                    break;
                }

                printf("Cantidad: ");
                if (scanf("%d", &cantidad) != 1) { 
                    int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
                    printf("Cantidad invalida.\n\n");
                    break;
                }
                { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

                if (cantidad <= 0) {
                    printf("La cantidad debe ser mayor a 0.\n\n");
                    break;
                }

                if (agregarDetallePedido(&pedido.detalles, &pedido.cantidadDetalles, codigo, cantidad, libros, cantLibros)) {
                    printf("Linea agregada correctamente.\n\n");
                }
                break;
            }

            case 2:
                if (pedido.cantidadDetalles == 0) {
                    printf("No hay lineas para eliminar.\n\n");
                    break;
                }
                
                mostrarDetallePedido(pedido.detalles, pedido.cantidadDetalles);
                printf("Numero de linea a eliminar: ");
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
                char *fechaInput = malloc(TAM_FECHA * sizeof(char));
                printf("Cedula del cliente: ");
                { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
                input(cedula, TAM_CEDULA);

                struct Cliente* cliente = buscarClientePorCedula(clientes, cantClientes, cedula);
                if (cliente == NULL) {
                    printf("Cliente no encontrado.\n\n");
                    free(cedula);
                    break;
                }

                
                printf("Fecha del pedido (dd/mm/yyyy): ");
                input(fechaInput, TAM_FECHA);
                if (strlen(fechaInput) == 0) {
                    printf("Fecha invalida. Operacion cancelada.\n\n");
                    free(cedula);
                    break;
                }
                

                // Llenar datos del pedido
                pedido.cedulaCliente = asignarString(cliente->cedula);
                pedido.nombreCliente = asignarString(cliente->nombre);
                pedido.fecha = asignarString(fechaInput);
                
                calcularTotalesPedido(pedido.detalles, pedido.cantidadDetalles, 
                                     &pedido.subtotalPedido, &pedido.impuesto, &pedido.totalPedido);
                
                if (generarPedido(&pedido, libros, &cantLibros, config)) {
                    mostrarPedidoCompleto(&pedido, config);
                    printf("Pedido generado exitosamente.\n\n");

                    free(cedula);
                    for (int i = 0; i < pedido.cantidadDetalles; i++) {
                        free(pedido.detalles[i]->codigoLibro);
                        free(pedido.detalles[i]->nombreLibro);
                        free(pedido.detalles[i]);
                    }
                    free(pedido.detalles);
                    free(pedido.cedulaCliente);
                    free(pedido.nombreCliente);
                    free(pedido.fecha);

                    free(codigo);

                    if (libros) {
                        for (int i = 0; i < cantLibros; i++) {
                            free(libros[i]->codigo);
                            free(libros[i]->nombre);
                            free(libros[i]->autor);
                            free(libros[i]);
                        }
                        free(libros);
                    }
                    if (clientes) {
                        for (int i = 0; i < cantClientes; i++) {
                            free(clientes[i]->cedula);
                            free(clientes[i]->nombre);
                            free(clientes[i]->telefono);
                            free(clientes[i]);
                        }
                        free(clientes);
                    }
                    return;
                }

                free(cedula);
                break;

            case 5:
                printf("Saliendo sin guardar...\n\n");
                break;
                
            default:
                printf("Opcion no válida.\n\n");
                break;
        }
    } while (opcion != 4 && opcion != 5);

    free(codigo);
    for (int i = 0; i < pedido.cantidadDetalles; i++) {
        free(pedido.detalles[i]->codigoLibro);
        free(pedido.detalles[i]->nombreLibro);
        free(pedido.detalles[i]);
    }
    free(pedido.detalles);

    free(pedido.cedulaCliente);
    free(pedido.nombreCliente);
    free(pedido.fecha);

    if (libros) {
        for (int i = 0; i < cantLibros; i++) {
            free(libros[i]->codigo);
            free(libros[i]->nombre);
            free(libros[i]->autor);
            free(libros[i]);
        }
        free(libros);
    }

    if (clientes) {
        for (int i = 0; i < cantClientes; i++) {
            free(clientes[i]->cedula);
            free(clientes[i]->nombre);
            free(clientes[i]->telefono);
            free(clientes[i]);
        }
        free(clientes);
    }
}

/*
    Nombre: menuEliminarPedido
    Entrada: ninguna
    Salida: void
    Objetivo:
        Listar pedidos, solicitar un ID y eliminarlo usando eliminarPedido,
        actualizando archivos y liberando la memoria restante.
*/
void menuEliminarPedido() {
    int cantPedidos = 0;
    struct Pedido** pedidos = cargarPedidos(&cantPedidos);

    if (cantPedidos == 0) {
        printf("No hay pedidos registrados.\n\n");
        printf("Presione ENTER para volver...");
        getchar();
        return;
    }

    char *idPedido = malloc(TAM_ID_PEDIDO * sizeof(char));
    { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

    printf("------- Eliminar Pedido -------\n\n");
    
    // Mostrar lista de pedidos
    printf("Pedidos disponibles:\n");
    printf("%-12s %-25s %-12s %-12s\n", "ID", "Cliente", "Fecha", "Total");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < cantPedidos; i++) {
        printf("%-12s %-25s %-12s $%-11.2f\n",
               pedidos[i]->idPedido,
               pedidos[i]->nombreCliente,
               pedidos[i]->fecha,
               pedidos[i]->totalPedido);
    }
    printf("\n");
    
    printf("Identificador del pedido a eliminar: ");
    input(idPedido, TAM_ID_PEDIDO);

    if (strlen(idPedido) == 0) {
        printf("ID de pedido no puede estar vacio.\n\n");
        free(idPedido);
        return;
    }

    if (!eliminarPedido(&pedidos, &cantPedidos, idPedido)) {
        printf("No se pudo eliminar el pedido.\n\n");
    }

    // Liberar memoria
    for (int i = 0; i < cantPedidos; i++) {
        for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
            free(pedidos[i]->detalles[j]->codigoLibro);
            free(pedidos[i]->detalles[j]->nombreLibro);
            free(pedidos[i]->detalles[j]);
        }
        free(pedidos[i]->detalles);
        free(pedidos[i]->idPedido);
        free(pedidos[i]->cedulaCliente);
        free(pedidos[i]->nombreCliente);
        free(pedidos[i]->fecha);
        free(pedidos[i]);
    }
    free(pedidos);
    free(idPedido);
}

/*
    Nombre: menuModificarPedido
    Entrada: puntero a Configuracion
    Salida: void
    Objetivo:
        Permitir editar un pedido existente: cambiar cantidades, agregar o
        eliminar líneas y guardar cambios afectando el inventario y archivos.
*/
void menuModificarPedido(struct Configuracion* config) {
    int cantPedidos = 0, cantLibros = 0;
    struct Pedido** pedidos = cargarPedidos(&cantPedidos);
    struct Libro**  libros  = cargarLibros(&cantLibros);

    if (cantPedidos == 0) {
        printf("No hay pedidos registrados.\n\n");
        printf("Presione ENTER para volver...");
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
        getchar();
        return;
    }

    printf("------- Modificar Pedido -------\n\n");
    printf("%-12s %-25s %-12s %-12s\n", "ID", "Cliente", "Fecha", "Total");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < cantPedidos; i++) {
        printf("%-12s %-25s %-12s $%-11.2f\n",
               pedidos[i]->idPedido, pedidos[i]->nombreCliente,
               pedidos[i]->fecha, pedidos[i]->totalPedido);
    }
    printf("\n");

    char* id = malloc(30 * sizeof(char));
    { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
    printf("Identificador del pedido a modificar: ");
    input(id, 30);
    if (id[0] == '\0') { printf("ID vacio.\n\n"); free(id); return; }

    int indicePedido = -1;
    for (int i = 0; i < cantPedidos; i++) {
        if (compararString(pedidos[i]->idPedido, id)) { indicePedido = i; break; }
    }
    if (indicePedido < 0) {
        printf("Pedido no encontrado.\n\n");
        free(id);
        return;
    }
    struct Pedido* ped = pedidos[indicePedido];

    int nOriginal = ped->cantidadDetalles;
    char** codigosOriginales = malloc(nOriginal * sizeof(char*));
    int* cantidadesOriginales = malloc(nOriginal * sizeof(int));
    for (int i = 0; i < nOriginal; i++) {
        codigosOriginales[i] = asignarString(ped->detalles[i]->codigoLibro);
        cantidadesOriginales[i] = ped->detalles[i]->cantidad;
    }

    int salir = 0;
    while (!salir) {
        printf("\n=== EDITAR PEDIDO %s ===\n", ped->idPedido);
        mostrarDetallePedido(ped->detalles, ped->cantidadDetalles);

        printf("1. Cambiar cantidad de una linea\n");
        printf("2. Agregar linea\n");
        printf("3. Eliminar linea\n");
        printf("4. Guardar cambios\n");
        printf("5. Cancelar sin guardar\n\n");
        printf("Seleccione una opcion: ");

        int op = 0;
        scanf("%d", &op);
        { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

        if (op == 1) {
            int num, nueva;
            printf("Numero de linea: ");
            scanf("%d", &num);
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            if (num < 1 || num > ped->cantidadDetalles) { printf("Fuera de rango.\n"); continue; }
            struct DetallePedido* d = ped->detalles[num-1];
            printf("Cantidad actual: %d\n", d->cantidad);
            printf("Nueva cantidad: ");
            scanf("%d", &nueva);
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            if (nueva > 0) {
                d->cantidad = nueva;
                d->subtotal = d->cantidad * d->precio;
                printf("Linea actualizada.\n");
            }
        }
        else if (op == 2) {
            char* codigo = malloc(20 * sizeof(char));
            int cant;
            printf("Codigo del libro: ");
            input(codigo, 20);
            printf("Cantidad: ");
            scanf("%d", &cant);
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            if (cant > 0) {
                agregarDetallePedido(&ped->detalles, &ped->cantidadDetalles,
                                     codigo, cant, libros, cantLibros);
            }
        }
        else if (op == 3) {
            int num;
            printf("Numero de linea a eliminar: ");
            scanf("%d", &num);
            { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }
            eliminarDetallePedido(&ped->detalles, &ped->cantidadDetalles, num);
        }
        else if (op == 4) {
            if (ped->cantidadDetalles == 0) {
                printf("No se puede guardar un pedido vacio.\n");
                continue;
            }

            // Recalcular totales
            calcularTotalesPedido(ped->detalles, ped->cantidadDetalles,
                                  &ped->subtotalPedido, &ped->impuesto, &ped->totalPedido);

            // Devolver inventario original
            for (int i = 0; i < nOriginal; i++) {
                struct Libro* lib = buscarLibroPorCodigo(libros, cantLibros, codigosOriginales[i]);
                if (lib) lib->cantidad += cantidadesOriginales[i];
            }
            // Descontar inventario actual
            for (int j = 0; j < ped->cantidadDetalles; j++) {
                struct Libro* lib = buscarLibroPorCodigo(libros, cantLibros, ped->detalles[j]->codigoLibro);
                if (lib) lib->cantidad -= ped->detalles[j]->cantidad;
            }

            actualizarTodosLibros(libros, &cantLibros);
            reescribirArchivosPedidos(pedidos, cantPedidos);

            printf("Pedido actualizado correctamente.\n");
            salir = 1;
        }
        else if (op == 5) {
            printf("Cancelando sin guardar...\n");
            salir = 1;
        }
    }

    // liberar memoria 
    for (int i = 0; i < nOriginal; i++) free(codigosOriginales[i]);
    free(codigosOriginales);
    free(cantidadesOriginales);

    if (libros) {
        for (int i = 0; i < cantLibros; i++) {
            free(libros[i]->codigo);
            free(libros[i]->nombre);
            free(libros[i]->autor);
            free(libros[i]);
        }
        free(libros);
    }

    if (pedidos) {
        for (int i = 0; i < cantPedidos; i++) {
            for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                free(pedidos[i]->detalles[j]->codigoLibro);
                free(pedidos[i]->detalles[j]->nombreLibro);
                free(pedidos[i]->detalles[j]);
            }
            free(pedidos[i]->detalles);
            free(pedidos[i]->idPedido);
            free(pedidos[i]->cedulaCliente);
            free(pedidos[i]->nombreCliente);
            free(pedidos[i]->fecha);
            free(pedidos[i]);
        }
        free(pedidos);
    }
}

/*
    Nombre: menuManejoInventario
    Entrada: ninguna
    Salida: void
    Objetivo:
        Permitir actualizar inventario leyendo un archivo con movimientos.
        Reescribe el archivo de libros y libera la memoria utilizada.
*/
void menuManejoInventario() {
    printf("------- Manejo de inventario -------\n\n");
    printf("Este menu permite modificar la cantidad de libros disponibles, cargando la informacion desde un archivo.\n\n");
    printf("Escriba el nombre de archivo o presione [Enter] para volver atras.\n\n> ");

    char *archivo = malloc(100 * sizeof(char));
    input(archivo, 100);
    if (archivo[0] == '\0') {
        free(archivo);
        return;
    }

    int cant;
    struct Libro** libros = cargarLibros(&cant);
    modificarInventario(libros, &cant, archivo);
    free(archivo);
    if (libros) {
        for (int i = 0; i < cant; i++) {
            free(libros[i]->codigo);
            free(libros[i]->nombre);
            free(libros[i]->autor);
            free(libros[i]);
        }
        free(libros);
    }
    return;
}

/*
    Nombre: menuEstadisticas
    Entrada: ninguna
    Salida: void
    Objetivo:
        Mostrar un submenú de reportes (ventas por año/mes, top clientes/libros,
        autor top por año) usando los datos de pedidos cargados.
*/
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
            input(anio, 8);
            if (anio[0] == '\0') {
                mostrarLibrosMasVendidos(pedidos, cantidadPedidos, NULL, topN);
            } else {
                mostrarLibrosMasVendidos(pedidos, cantidadPedidos, anio, topN);
            }
            free(anio);
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
    if (pedidos) {
        for (int i = 0; i < cantidadPedidos; i++) {
            for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                free(pedidos[i]->detalles[j]->codigoLibro);
                free(pedidos[i]->detalles[j]->nombreLibro);
                free(pedidos[i]->detalles[j]);
            }
            free(pedidos[i]->detalles);
            free(pedidos[i]->idPedido);
            free(pedidos[i]->cedulaCliente);
            free(pedidos[i]->nombreCliente);
            free(pedidos[i]->fecha);
            free(pedidos[i]);
        }
        free(pedidos);
    }
}

/*
    Nombre: menuConsultaPedidos
    Entrada: puntero a Configuracion
    Salida: void
    Objetivo:
        Listar pedidos y permitir seleccionar uno para ver su comprobante
        completo (encabezado, detalle y totales).
*/
void menuConsultaPedidos(struct Configuracion* config) {
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
        mostrarPedidoCompleto(seleccionado, config);

        printf("Presione ENTER para volver a la lista...");
        getchar();
        printf("\n");
    }
    if (pedidos) {
        for (int i = 0; i < cantidadPedidos; i++) {
            for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                free(pedidos[i]->detalles[j]->codigoLibro);
                free(pedidos[i]->detalles[j]->nombreLibro);
                free(pedidos[i]->detalles[j]);
            }
            free(pedidos[i]->detalles);
            free(pedidos[i]->idPedido);
            free(pedidos[i]->cedulaCliente);
            free(pedidos[i]->nombreCliente);
            free(pedidos[i]->fecha);
            free(pedidos[i]);
        }
        free(pedidos);
    }
}

/*
    Nombre: menuEliminarLibro
    Entrada: ninguna
    Salida: void
    Objetivo:
        Solicitar un código, validar que exista y no esté referido por pedidos,
        y eliminar el libro del catálogo/archivo si procede.
*/
void menuEliminarLibro() {
    int cantLibros = 0, cantPedidos = 0;
    struct Libro** libros = cargarLibros(&cantLibros);
    struct Pedido** pedidos = cargarPedidos(&cantPedidos);

    if (cantLibros == 0) {
        printf("No hay libros para eliminar.\n\n");
        if (pedidos) {
            for (int i = 0; i < cantPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
        return;
    }

    char *codigo = malloc(20 * sizeof(char));
    { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

    printf("------- Eliminar Libro -------\n\n");
    printf("Codigo del libro: ");
    input(codigo, 20);

    if (strlen(codigo) == 0) {
        printf("El codigo no puede estar vacio.\n\n");
        free(codigo);
        if (libros) {
            for (int i = 0; i < cantLibros; i++) {
                free(libros[i]->codigo);
                free(libros[i]->nombre);
                free(libros[i]->autor);
                free(libros[i]);
            }
            free(libros);
        }
        if (pedidos) {
            for (int i = 0; i < cantPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
        return;
    }

    if (!eliminarLibro(&libros, &cantLibros, pedidos, cantPedidos, codigo)) {
        free(codigo);
        if (libros) {
            for (int i = 0; i < cantLibros; i++) {
                free(libros[i]->codigo);
                free(libros[i]->nombre);
                free(libros[i]->autor);
                free(libros[i]);
            }
            free(libros);
        }
        if (pedidos) {
            for (int i = 0; i < cantPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
        return;
    }

    free(codigo);
    if (pedidos) {
        for (int i = 0; i < cantPedidos; i++) {
            for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                free(pedidos[i]->detalles[j]->codigoLibro);
                free(pedidos[i]->detalles[j]->nombreLibro);
                free(pedidos[i]->detalles[j]);
            }
            free(pedidos[i]->detalles);
            free(pedidos[i]->idPedido);
            free(pedidos[i]->cedulaCliente);
            free(pedidos[i]->nombreCliente);
            free(pedidos[i]->fecha);
            free(pedidos[i]);
        }
        free(pedidos);
    }

    if (libros) {
        for (int i = 0; i < cantLibros; i++) {
            free(libros[i]->codigo);
            free(libros[i]->nombre);
            free(libros[i]->autor);
            free(libros[i]);
        }
        free(libros);
    }
}

/*
    Nombre: menuEliminarCliente
    Entrada: ninguna
    Salida: void
    Objetivo:
        Solicitar cédula y eliminar al cliente si no tiene pedidos asociados.
        Maneja la liberación de estructuras en memoria.
*/
void menuEliminarCliente() {
    int cantClientes = 0, cantPedidos = 0;
    struct Cliente** clientes = cargarClientes(&cantClientes);
    struct Pedido** pedidos   = cargarPedidos(&cantPedidos);

    if (cantClientes == 0) {
        printf("No hay clientes para eliminar.\n\n");
        if (pedidos) {
            for (int i = 0; i < cantPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
        return;
    }

    char *cedula = malloc(TAM_CEDULA * sizeof(char));
    { int ch; while ((ch = getchar()) != '\n' && ch != EOF) {} }

    printf("------- Eliminar Cliente -------\n\n");
    printf("Cedula del cliente: ");
    input(cedula, TAM_CEDULA);

    if (strlen(cedula) == 0) {
        printf("La cedula no puede estar vacia.\n\n");
        free(cedula);
        if (clientes) {
            for (int i = 0; i < cantClientes; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        if (pedidos) {
            for (int i = 0; i < cantPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
        return;
    }

    if (!eliminarCliente(&clientes, &cantClientes, pedidos, cantPedidos, cedula)) {
        free(cedula);
        if (clientes) {
            for (int i = 0; i < cantClientes; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        if (pedidos) {
            for (int i = 0; i < cantPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
        return;
    }
    free(cedula);
    if (clientes) {
        for (int i = 0; i < cantClientes; i++) {
            free(clientes[i]->cedula);
            free(clientes[i]->nombre);
            free(clientes[i]->telefono);
            free(clientes[i]);
        }
        free(clientes);
    }
    if (pedidos) {
        for (int i = 0; i < cantPedidos; i++) {
            for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                free(pedidos[i]->detalles[j]->codigoLibro);
                free(pedidos[i]->detalles[j]->nombreLibro);
                free(pedidos[i]->detalles[j]);
            }
            free(pedidos[i]->detalles);
            free(pedidos[i]->idPedido);
            free(pedidos[i]->cedulaCliente);
            free(pedidos[i]->nombreCliente);
            free(pedidos[i]->fecha);
            free(pedidos[i]);
        }
        free(pedidos);
    }
}

/*
    Nombre: menuLogin
    Entrada: puntero a Configuracion
    Salida: true si autenticación válida, false en caso contrario
    Objetivo:
        Pedir usuario y contraseña y validar contra la configuración cargada.
*/
bool menuLogin(struct Configuracion* config) {
    char *usuario = malloc(30 * sizeof(char));
    char *contrasena = malloc(30 * sizeof(char));
    printf("------- Autenticacion -------\n\n");
    printf("Nombre de usuario: ");
    input(usuario, 30);
    printf("\nContraseña: ");
    input(contrasena, 30);
    printf("\n\n");

    if (verificarAdmin(usuario, contrasena, config)) {
        printf("Acceso concedido.\n\n");
        free(usuario);
        free(contrasena);
        return true;
    }
    printf("Usuario o contraseña incorrectos.\n\n");
    free(usuario);
    free(contrasena);
    return false;    
}

/*
    Nombre: menuAdministrativo
    Entrada: puntero a Configuracion
    Salida: void
    Objetivo:
        Mostrar el menú administrativo y despachar a las operaciones de gestión
        de libros, clientes y pedidos. Recursivo para volver al menú.
*/
void menuAdministrativo(struct Configuracion* config) {
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
            menuCrearPedido(config);
            break;
        case 7:
            menuModificarPedido(config);
            break;
        case 8:
            menuEliminarPedido();
            break;
        case 9:
            menuConsultaPedidos(config);
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
    menuAdministrativo(config);
}

/*
    Nombre: menuMostrarCatalogo
    Entrada: ninguna
    Salida: void
    Objetivo:
        Cargar y mostrar en consola el catálogo de libros con sus datos.
*/
void menuMostrarCatalogo() {
    int cant;
    struct Libro** libros = cargarLibros(&cant);
    printf("------- Catalogo -------\n\n");
    if (cant == 0) {
        printf("No hay libros disponibles.\n\n");
        if (libros) free(libros);
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
    if (libros) {
        for (int i = 0; i < cant; i++) {
            free(libros[i]->codigo);
            free(libros[i]->nombre);
            free(libros[i]->autor);
            free(libros[i]);
        }
        free(libros);
    }
}

/*
    Nombre: menuConsultaCliente
    Entrada: ninguna
    Salida: void
    Objetivo:
        Pedir cédula, mostrar datos del cliente y listar sus pedidos
        (con cantidad de libros por pedido).
*/
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
        free(cedula);
        if (clientes) {
            for (int i = 0; i < cantidadClientes; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        if (pedidos) {
            for (int i = 0; i < cantidadPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
        return;
    }

    // Buscar cliente
    struct Cliente* cliente = buscarClientePorCedula(clientes, cantidadClientes, cedula);
    if (cliente == NULL) {
        printf("Cliente no encontrado.\n\n");
        free(cedula);
        if (clientes) {
            for (int i = 0; i < cantidadClientes; i++) {
                free(clientes[i]->cedula);
                free(clientes[i]->nombre);
                free(clientes[i]->telefono);
                free(clientes[i]);
            }
            free(clientes);
        }
        if (pedidos) {
            for (int i = 0; i < cantidadPedidos; i++) {
                for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                    free(pedidos[i]->detalles[j]->codigoLibro);
                    free(pedidos[i]->detalles[j]->nombreLibro);
                    free(pedidos[i]->detalles[j]);
                }
                free(pedidos[i]->detalles);
                free(pedidos[i]->idPedido);
                free(pedidos[i]->cedulaCliente);
                free(pedidos[i]->nombreCliente);
                free(pedidos[i]->fecha);
                free(pedidos[i]);
            }
            free(pedidos);
        }
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

    free(cedula);
    if (clientes) {
        for (int i = 0; i < cantidadClientes; i++) {
            free(clientes[i]->cedula);
            free(clientes[i]->nombre);
            free(clientes[i]->telefono);
            free(clientes[i]);
        }
        free(clientes);
    }
    if (pedidos) {
        for (int i = 0; i < cantidadPedidos; i++) {
            for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
                free(pedidos[i]->detalles[j]->codigoLibro);
                free(pedidos[i]->detalles[j]->nombreLibro);
                free(pedidos[i]->detalles[j]);
            }
            free(pedidos[i]->detalles);
            free(pedidos[i]->idPedido);
            free(pedidos[i]->cedulaCliente);
            free(pedidos[i]->nombreCliente);
            free(pedidos[i]->fecha);
            free(pedidos[i]);
        }
        free(pedidos);
    }
}

/*
    Nombre: menuOpcionesPrincipales
    Entrada: ninguna
    Salida: void
    Objetivo:
        Submenú de opciones públicas: ver catálogo y consultar cliente.
        Vuelve a llamarse recursivamente para permanecer en el menú.
*/
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

/*
    Nombre: menuPrincipal
    Entrada: ninguna
    Salida: void
    Objetivo:
        Cargar configuración, presentar el menú principal y despachar hacia
        el menú administrativo (con login) o las opciones principales.
        Maneja la liberación de la configuración al salir de cada ciclo.
*/
void menuPrincipal() {
    struct Configuracion* config = cargarConfiguracion();
    if (!config) {
        printf("Error al cargar la configuración. Saliendo...\n");
        exit(1);
    }

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
            auth = menuLogin(config);
            if (auth) {
                menuAdministrativo(config);
            }
            break;
        case 2:
            menuOpcionesPrincipales();
            break;
        case 3:
            liberarConfiguracion(config);
            exit(0);
            break;
        default:
            printf("Opcion no valida.\n\n");
            break;
    }
    
    liberarConfiguracion(config);
    menuPrincipal();
}
