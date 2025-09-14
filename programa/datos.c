#include "cliente.h"
#include "datos.h"
#include "libro.h"
#include "auxiliares.h"
#include "pedido.h"
#include "registro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// CLIENTES
struct Cliente** cargarClientes(int* cant) {
    int cantidadLineas;
    char** clientesTxt = leerArchivo(CLIENTES_TXT, &cantidadLineas);
    struct Cliente** clientes = malloc(cantidadLineas * sizeof(struct Cliente*));
    if (clientesTxt == NULL) {
        *cant = 0;
        return NULL;
    }
    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(clientesTxt[i], ';', 3);
        struct Cliente* cliente = malloc(sizeof(struct Cliente));
        cliente->cedula = asignarString(info[0]);
        cliente->nombre = asignarString(info[1]);
        cliente->telefono = asignarString(info[2]);
        clientes[i] = cliente;
        for (int j = 0; j < 3; j++) free(info[j]);
        free(info);
    }
    *cant = cantidadLineas;
    return clientes;
}

int existeCedula(struct Cliente** clientes, int cantidad, char* cedula) {
    for (int i = 0; i < cantidad; i++) {
        if (compararString(clientes[i]->cedula, cedula)) {
            return 1;
        }
    }
    return 0;
}

int validarTelefono(char* telefono) {
    int len = strlen(telefono);
    if (len < 7) {
        return 0;
    } 
    for (int i = 0; i < len; i++) {
        if (telefono[i] < '0' || telefono[i] > '9') {
            return 0;
        }
    }
    return 1;
}

bool registrarCliente(struct Cliente** listaClientes, struct Cliente* cliente, int cantidadClientes) {
    for (int i = 0; i < cantidadClientes; i++) {
        if (compararString(listaClientes[i]->cedula, cliente->cedula)) {
            printf("Error: Ya existe un cliente con esa cedula.\n\n");
            return false;
        }
    }

    listaClientes = (struct Cliente**)realloc(listaClientes, (cantidadClientes + 1) * sizeof(struct Cliente*));
    if (listaClientes == NULL) {
        return false;
    }
    listaClientes[cantidadClientes] = cliente;
    cantidadClientes++;

    FILE *archivo = fopen(CLIENTES_TXT, "a");
    if (cantidadClientes == 1) {
        fprintf(archivo, "%s;%s;%s", cliente->cedula, cliente->nombre, cliente->telefono);
    } else {
        fprintf(archivo, "\n%s;%s;%s", cliente->cedula, cliente->nombre, cliente->telefono);
    }
    fclose(archivo);
    return true;
}

void actualizarTodosClientes(struct Cliente** clientes, int* cantidadClientes) {
    FILE *archivo = fopen(CLIENTES_TXT, "w");
    for (int i = 0; i < *cantidadClientes; i++) {
        if (i == 0) {
            fprintf(archivo, "%s;%s;%s", clientes[i]->cedula, clientes[i]->nombre, clientes[i]->telefono);
        } else {
            fprintf(archivo, "\n%s;%s;%s", clientes[i]->cedula, clientes[i]->nombre, clientes[i]->telefono);
        }
    }
    fclose(archivo);
}

static int buscarIndiceCliente(struct Cliente** clientes, int cantClientes, const char* cedula) {
    for (int i = 0; i < cantClientes; i++) {
        if (compararString(clientes[i]->cedula, (char*)cedula)) return i;
    }
    return -1;
}

bool tienePedidosCliente(struct Pedido** pedidos, int cantPedidos, const char* cedula) {
    for (int i = 0; i < cantPedidos; i++) {
        if (compararString(pedidos[i]->cedulaCliente, (char*)cedula)) {
            return true;
        }
    }
    return false;
}

bool eliminarCliente(struct Cliente*** clientes, int* cantClientes,
                     struct Pedido** pedidos, int cantPedidos,
                     const char* cedula) {
    if (clientes == NULL || *clientes == NULL || *cantClientes <= 0) {
        printf("No hay clientes cargados.\n\n");
        return false;
    }

    if (tienePedidosCliente(pedidos, cantPedidos, cedula)) {
        printf("No se puede eliminar: el cliente tiene pedidos asociados.\n\n");
        return false;
    }

    int ind = buscarIndiceCliente(*clientes, *cantClientes, cedula);
    if (ind < 0) {
        printf("Cliente no encontrado.\n\n");
        return false;
    }

    // liberar nodo y compactar
    free((*clientes)[ind]);
    for (int i = ind; i < (*cantClientes) - 1; i++) {
        (*clientes)[i] = (*clientes)[i + 1];
    }
    (*cantClientes)--;

    if (*cantClientes > 0) {
        void* nuevo = realloc(*clientes, (*cantClientes) * sizeof(struct Cliente*));
        if (nuevo != NULL) *clientes = (struct Cliente**)nuevo; 
    } else {
        free(*clientes);
        *clientes = NULL;
    }

    actualizarTodosClientes(*clientes, cantClientes);  
    printf("Cliente eliminado correctamente.\n\n");
    return true;
}

/*
    Nombre: leerArchivo
    Entrada: Nombre del archivo (char*) y cantidad de lineas (int*)
    Salida: Lineas del archivo (char**)
    Objetivo: 
        Leer el contenido de un archivo linea por linea y agregarlo a un arreglo de lineas
        Para esto, se usa fgets para recorrer linea a linea, realloc para ir redimensionando el arreglo de
        lineas, malloc para asignar memoria en el espacio del arreglo y strcpy para copiar la linea leida.
*/
char** leerArchivo(char* nombreArchivo, int* cantidadLineas) {
    char** lineas = NULL;
    int cantidad = 0;
    char* linea = malloc(150 * sizeof(char));
    FILE *archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        *cantidadLineas = 0;
        printf("Error al abrir el archivo.\n\n");
        return NULL;
    }

    while(fgets(linea, 150, archivo)) {
        linea[strcspn(linea, "\n")] = 0;
        cantidad++;
        lineas = (char**)realloc(lineas, cantidad * sizeof(char*));
        lineas[cantidad-1] = (char*)malloc(strlen(linea) + 1);
        copiarString(lineas[cantidad-1], linea);
    }

    fclose(archivo);
    *cantidadLineas = cantidad;
    return lineas;
}


bool verificarAdmin(char* usuario, char* contrasena) {
    int* cantidadLineas;
    char** credenciales = leerArchivo("acceso.txt", cantidadLineas);

    if (credenciales == NULL) {
        printf("Error al leer el archivo.\n");
        return false;
    }
    
    for (int i = 0; i < *cantidadLineas; i++) {
        char** info = separarTexto(credenciales[i], ';', 2);
        char* infoUsuario = info[0];
        char* infoContrasena = info[1];
        if (compararString(infoUsuario, usuario) == true && compararString(infoContrasena, contrasena) == true) {
            return true;
        }
    }
    return false;
}

/*
    Nombre: separarTexto
    Entrada: Texto (char*) y delimitador (char) y cantidad de separaciones (int)
    Salida: Texto separado (char**)
    Objetivo:
        Separar el texto por el delimitador y almacenarlo en un arreglo de cadenas de caracteres
        Para esto, se crea un arreglo de la cantidad de separaciones, se copia el texto en una variable temporal,
        se copia el texto en el arreglo de cadenas de caracteres y se retorna el arreglo despues de procesar todo.
        Como el delimitador nunca está al final de la cadena, se utiliza un while para recorrer la cadena y
        el último extracto de texto se agrega fuera del while.
*/
char** separarTexto(char* texto, char delimitador, int cantidad) {
    char** array = malloc(cantidad * sizeof(char*));
    char* temp = malloc(100 * sizeof(char));
    int indiceArray = 0;
    int indiceTexto = 0;
    int indiceTemp = 0;
    while (texto[indiceTexto] != '\0') {
        if (texto[indiceTexto] == ';') {
            indiceTexto++;
            temp[indiceTemp] = '\0';
            array[indiceArray] = (char*)malloc(strlen(temp) + 1);
            copiarString(array[indiceArray], temp);
            indiceArray++;
            temp[0] = '\0';
            indiceTemp = 0;
            continue;
        }
        temp[indiceTemp] = texto[indiceTexto]; 
        indiceTexto++;
        indiceTemp++;
    }
    temp[indiceTemp] = '\0';
    array[indiceArray] = (char*)malloc(strlen(temp) + 1);
    copiarString(array[indiceArray], temp);
    return array;
};


/* GESTION DE LIBROS */

struct Libro** cargarLibros(int* cant) {
    int cantidadLineas;
    char** librosTxt = leerArchivo("libros.txt", &cantidadLineas);
    struct Libro** libros = malloc(cantidadLineas * sizeof(struct Libro*));

    if (librosTxt == NULL) {
        printf("Error al leer el archivo.\n");
        return NULL;
    }
    
    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(librosTxt[i], ';', 5);
        struct Libro* libro = malloc(sizeof(struct Libro));
        libro->codigo = asignarString(info[0]);
        libro->nombre = asignarString(info[1]);
        libro->autor  = asignarString(info[2]);
        libro->precio = atof(info[3]);
        libro->cantidad = atoi(info[4]);
        libros[i] = libro;

        for (int j = 0; j < 5; j++) free(info[j]);
        free(info);
    }
    *cant = cantidadLineas;
    return libros;
}


bool registrarLibro(struct Libro** listaLibros, struct Libro* libro, int cantidadLibros) {
    for (int i = 0; i < cantidadLibros; i++) {
        if (strcmp(listaLibros[i]->codigo, libro->codigo) == 0) {
            printf("Error: Ya existe un libro con ese codigo.\n\n");
            return false;
        }
    }

    listaLibros = (struct Libro**)realloc(listaLibros, (cantidadLibros + 1) * sizeof(struct Libro*));
    if (listaLibros == NULL) {
        return false;
    }
    listaLibros[cantidadLibros] = libro;
    cantidadLibros++;

    FILE *archivo = fopen("libros.txt", "a");
    if (cantidadLibros == 1) {
        fprintf(archivo, "%s;%s;%s;%.2f;%d", libro->codigo, libro->nombre, libro->autor, libro->precio, libro->cantidad);
    }
    fprintf(archivo, "\n%s;%s;%s;%.2f;%d", libro->codigo, libro->nombre, libro->autor, libro->precio, libro->cantidad);
    fclose(archivo);

    return true;
}

void actualizarTodosLibros(struct Libro** libros, int* cantidadLibros) {
    FILE *limpiar = fopen("libros.txt", "w");
    fclose(limpiar);

    FILE *archivo = fopen("libros.txt", "a");

    for (int i = 0; i < *cantidadLibros; i++)
    {
        if (i == 0) {
            fprintf(archivo, "%s;%s;%s;%.2f;%d", libros[i]->codigo, libros[i]->nombre, libros[i]->autor, libros[i]->precio, libros[i]->cantidad);
        }
        else {
            fprintf(archivo, "\n%s;%s;%s;%.2f;%d", libros[i]->codigo, libros[i]->nombre, libros[i]->autor, libros[i]->precio, libros[i]->cantidad);
        }
    }  
    fclose(archivo);
}

bool modificarInventario(struct Libro** libros, int* cantidadLibros, char* archivo) {
    int cantidadLineas;
    char** contenido = leerArchivo(archivo, &cantidadLineas);
    if (contenido == NULL) {
        return false;
    }

    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(contenido[i], ';', 2);
        for (int j = 0; j < *cantidadLibros; j++) {
            if (compararString(libros[j]->codigo, info[0]) == true) {
                int count = libros[j]->cantidad + atoi(info[1]);
                if (count > 0) {
                    libros[j]->cantidad = count;
                }
                else {
                    printf("Linea %d no procesada, el stock quedaria negativo.\n", i+1);
                }
            }
        }
    }
    printf("\n");
    actualizarTodosLibros(libros, cantidadLibros);
    return true;
}

bool tienePedidosLibro(struct Pedido** pedidos, int cantPedidos, const char* codigoLibro) {
    for (int i = 0; i < cantPedidos; i++) {
        for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
            if (compararString(pedidos[i]->detalles[j]->codigoLibro, (char*)codigoLibro)) {
                return true;
            }
        }
    }
    return false;
}

static int buscarIndiceLibro(struct Libro** libros, int cantLibros, const char* codigo) {
    for (int i = 0; i < cantLibros; i++) {
        if (compararString(libros[i]->codigo, (char*)codigo)) return i;
    }
    return -1;
}

bool eliminarLibro(struct Libro*** libros, int* cantLibros,
                   struct Pedido** pedidos, int cantPedidos,
                   const char* codigoLibro) {
    if (libros == NULL || *libros == NULL || *cantLibros <= 0) {
        printf("No hay libros cargados.\n\n");
        return false;
    }

    if (tienePedidosLibro(pedidos, cantPedidos, codigoLibro)) {
        printf("No se puede eliminar: el libro aparece en pedidos.\n\n");
        return false;
    }

    int ind = buscarIndiceLibro(*libros, *cantLibros, codigoLibro);
    if (ind < 0) {
        printf("Libro no encontrado.\n\n");
        return false;
    }

    // liberar nodo y compactar
    free((*libros)[ind]);
    for (int i = ind; i < (*cantLibros) - 1; i++) {
        (*libros)[i] = (*libros)[i + 1];
    }
    (*cantLibros)--;

    if (*cantLibros > 0) {
        void* nuevo = realloc(*libros, (*cantLibros) * sizeof(struct Libro*));
        if (nuevo != NULL) *libros = (struct Libro**)nuevo;  // si falla, mantenemos puntero viejo (ya compactado)
    } else {
        free(*libros);
        *libros = NULL;
    }

    actualizarTodosLibros(*libros, cantLibros);  // OK con 0 items
    printf("Libro eliminado correctamente.\n\n");
    return true;
}

// PEDIDOS
struct Libro* buscarLibroPorCodigo(struct Libro** libros, int cantidad, char* codigo) {
    for (int i = 0; i < cantidad; i++) {
        if (compararString(libros[i]->codigo, codigo)) {
            return libros[i];
        }
    }
    return NULL;
}

struct Cliente* buscarClientePorCedula(struct Cliente** clientes, int cantidad, char* cedula) {
    for (int i = 0; i < cantidad; i++) {
        if (compararString(clientes[i]->cedula, cedula)) {
            return clientes[i];
        }
    }
    return NULL;
}

bool agregarDetallePedido(struct DetallePedido*** detalles, int* cantidadDetalles, char* codigoLibro, int cantidad, struct Libro** libros, int cantLibros) {
    struct Libro* libro = buscarLibroPorCodigo(libros, cantLibros, codigoLibro);
    if (libro == NULL) {
        printf("Error: Libro no encontrado.\n\n");
        return false;
    }
    
    if (libro->cantidad < cantidad) {
        printf("Error: Stock insuficiente. Disponible: %d\n\n", libro->cantidad);
        return false;
    }

    // Verificar si el libro ya existe en el pedido
    for (int i = 0; i < *cantidadDetalles; i++) {
        if (compararString((*detalles)[i]->codigoLibro, codigoLibro)) {
            if (libro->cantidad < ((*detalles)[i]->cantidad + cantidad)) {
                printf("Error: Stock insuficiente. Disponible: %d, En pedido: %d\n\n", 
                       libro->cantidad, (*detalles)[i]->cantidad);
                return false;
            }
            (*detalles)[i]->cantidad += cantidad;
            (*detalles)[i]->subtotal = (*detalles)[i]->cantidad * (*detalles)[i]->precio;
            printf("Cantidad actualizada. Nueva cantidad: %d\n\n", (*detalles)[i]->cantidad);
            return true;
        }
    }

    // Agregar nuevo detalle
    *detalles = realloc(*detalles, (*cantidadDetalles + 1) * sizeof(struct DetallePedido*));
    (*detalles)[*cantidadDetalles] = malloc(sizeof(struct DetallePedido));

    (*detalles)[*cantidadDetalles]->codigoLibro = asignarString(libro->codigo);
    (*detalles)[*cantidadDetalles]->nombreLibro = asignarString(libro->nombre);
    (*detalles)[*cantidadDetalles]->precio = libro->precio;
    (*detalles)[*cantidadDetalles]->cantidad = cantidad;
    (*detalles)[*cantidadDetalles]->subtotal = cantidad * libro->precio;
    
    (*cantidadDetalles)++;
    return true;
}

bool eliminarDetallePedido(struct DetallePedido*** detalles, int* cantidadDetalles, int numeroLinea) {
    if (numeroLinea < 1 || numeroLinea > *cantidadDetalles) {
        printf("Error: Numero de linea invalido.\n\n");
        return false;
    }
    
    int indice = numeroLinea - 1;
    free((*detalles)[indice]);
    
    for (int i = indice; i < *cantidadDetalles - 1; i++) {
        (*detalles)[i] = (*detalles)[i + 1];
    }
    
    (*cantidadDetalles)--;
    *detalles = realloc(*detalles, (*cantidadDetalles) * sizeof(struct DetallePedido*));
    
    printf("Linea eliminada correctamente.\n\n");
    return true;
}

void calcularTotalesPedido(struct DetallePedido** detalles, int cantidadDetalles, float* subtotal, float* impuesto, float* total) {
    *subtotal = 0;
    for (int i = 0; i < cantidadDetalles; i++) {
        *subtotal += detalles[i]->subtotal;
    }
    *impuesto = *subtotal * IMPUESTO_VENTA;
    *total = *subtotal + *impuesto;
}

char* generarIdPedido() {
    static char *id = NULL;
    if (!id) {
        id = malloc(TAM_ID_PEDIDO * sizeof(char));
        time_t t = time(NULL);
        snprintf(id, TAM_ID_PEDIDO, "%ld", t % 100000);
    }
    return id;
}

void mostrarDetallePedido(struct DetallePedido** detalles, int cantidadDetalles) {
    if (cantidadDetalles == 0) {
        printf("No hay lineas en el pedido.\n\n");
        return;
    }
    
    printf("=== DETALLE DEL PEDIDO ===\n");
    printf("%-3s %-15s %-30s %-8s %-8s %-10s\n", "No.", "Codigo", "Nombre", "Precio", "Cant.", "Subtotal");
    printf("------------------------------------------------------------------------\n");
    
    for (int i = 0; i < cantidadDetalles; i++) {
        printf("%-3d %-15s %-30s $%-7.2f %-8d $%-9.2f\n", 
               i + 1,
               detalles[i]->codigoLibro,
               detalles[i]->nombreLibro,
               detalles[i]->precio,
               detalles[i]->cantidad,
               detalles[i]->subtotal);
    }
    printf("\n");
}

bool generarPedido(struct Pedido* pedido, struct Libro** libros, int* cantLibros) {
    // Descontar del stock
    for (int i = 0; i < pedido->cantidadDetalles; i++) {
        struct Libro* libro = buscarLibroPorCodigo(libros, *cantLibros, pedido->detalles[i]->codigoLibro);
        if (libro != NULL) {
            libro->cantidad -= pedido->detalles[i]->cantidad;
        }
    }
    
    // Actualizar archivo de libros
    actualizarTodosLibros(libros, cantLibros);
    
    // Guardar pedido (encabezado) en archivo de pedidos
    FILE *archivoPedidos = fopen(PEDIDOS_TXT, "a");
    if (archivoPedidos == NULL) {
        printf("Error al guardar el pedido.\n");
        return false;
    }
    
    fprintf(archivoPedidos, "%s;%s;%s;%s;%.2f;%.2f;%.2f\n", 
            pedido->idPedido, pedido->cedulaCliente, pedido->nombreCliente, 
            pedido->fecha, pedido->subtotalPedido, pedido->impuesto, pedido->totalPedido);
    fclose(archivoPedidos);
    
    // Guardar detalles en archivo separado
    FILE *archivoDetalles = fopen(DETALLES_TXT, "a");
    if (archivoDetalles == NULL) {
        printf("Error al guardar los detalles del pedido.\n");
        return false;
    }
    
    for (int i = 0; i < pedido->cantidadDetalles; i++) {
        fprintf(archivoDetalles, "%s;%s;%s;%.2f;%d;%.2f\n",
                pedido->idPedido, pedido->detalles[i]->codigoLibro, pedido->detalles[i]->nombreLibro,
                pedido->detalles[i]->precio, pedido->detalles[i]->cantidad, pedido->detalles[i]->subtotal);
    }
    fclose(archivoDetalles);
    
    return true;
}

void mostrarPedidoCompleto(struct Pedido* pedido) {
    printf("\n");
    printf("===============================================\n");
    printf("                COMERCIO\n");
    printf("                COMERCIO\n");
    printf("===============================================\n");
    printf("Pedido No: %s\n", pedido->idPedido);
    printf("Fecha: %s\n", pedido->fecha);
    printf("Cliente: %s (%s)\n", pedido->nombreCliente, pedido->cedulaCliente);
    printf("===============================================\n\n");
    
    mostrarDetallePedido(pedido->detalles, pedido->cantidadDetalles);
    
    printf("-----------------------------------------------\n");
    printf("Subtotal:        $%.2f\n", pedido->subtotalPedido);
    printf("Impuesto (13%%):  $%.2f\n", pedido->impuesto);
    printf("TOTAL:           $%.2f\n", pedido->totalPedido);
    printf("===============================================\n\n");
}

struct Pedido** cargarPedidos(int* cant) {
    int cantidadLineas;
    char** pedidosTxt = leerArchivo(PEDIDOS_TXT, &cantidadLineas);
    
    if (pedidosTxt == NULL) {
        *cant = 0;
        return NULL;
    }
    
    struct Pedido** pedidos = malloc(cantidadLineas * sizeof(struct Pedido*));
    
    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(pedidosTxt[i], ';', 7);
        struct Pedido* pedido = malloc(sizeof(struct Pedido));
        
        pedido->idPedido = asignarString(info[0]);
        pedido->cedulaCliente = asignarString(info[1]);
        pedido->nombreCliente = asignarString(info[2]);
        pedido->fecha = asignarString(info[3]);
        pedido->subtotalPedido = atof(info[4]);
        pedido->impuesto = atof(info[5]);
        pedido->totalPedido = atof(info[6]);
        
        // Cargar detalles del pedido
        pedido->detalles = cargarDetallesPorPedido(pedido->idPedido, &pedido->cantidadDetalles);
        
        pedidos[i] = pedido;
        
        for (int j = 0; j < 7; j++) free(info[j]);
        free(info);
    }
    
    *cant = cantidadLineas;
    return pedidos;
}

struct DetallePedido** cargarDetallesPorPedido(char* idPedido, int* cant) {
    int cantidadLineas;
    char** detallesTxt = leerArchivo(DETALLES_TXT, &cantidadLineas);
    
    if (detallesTxt == NULL) {
        *cant = 0;
        return NULL;
    }
    
    struct DetallePedido** detalles = NULL;
    int cantidadDetalles = 0;
    
    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(detallesTxt[i], ';', 6);
        
        // Solo cargar detalles que pertenecen a este pedido
        if (compararString(info[0], idPedido)) {
            detalles = realloc(detalles, (cantidadDetalles + 1) * sizeof(struct DetallePedido*));
            detalles[cantidadDetalles] = malloc(sizeof(struct DetallePedido));
            
            detalles[cantidadDetalles]->codigoLibro = asignarString(info[1]);
            detalles[cantidadDetalles]->nombreLibro = asignarString(info[2]);
            detalles[cantidadDetalles]->precio = atof(info[3]);
            detalles[cantidadDetalles]->cantidad = atoi(info[4]);
            detalles[cantidadDetalles]->subtotal = atof(info[5]);
            
            cantidadDetalles++;
        }
        
        for (int j = 0; j < 6; j++) free(info[j]);
        free(info);
    }
    
    *cant = cantidadDetalles;
    return detalles;
}

// Estadisticas


// Auxiliares
static int buscarRegistro(Registro* arr, int n, const char* clave, const char* descripcionOpcional) {
    for (int i = 0; i < n; i++) {
        if (compararString(arr[i].clave, (char*)clave)) {
            if (descripcionOpcional == NULL) {
                return i; 
            } else if (compararString(arr[i].descripcion, (char*)descripcionOpcional)) {
                return i; 
            }
        }
    }
    return -1;
}

// Toma los últimos 4 caracteres de la fecha como año
static void anioDeFecha(const char* fecha, char* anio) {
    int len = (int)strlen(fecha);
    if (len >= 4) {
        anio[0] = fecha[len-4];
        anio[1] = fecha[len-3];
        anio[2] = fecha[len-2];
        anio[3] = fecha[len-1];
        anio[4] = '\0';
    } else {
        copiarString(anio, "????");
    }
}

static void mesAnioDeFecha(const char* fecha, char* out) {
    if (!fecha || (int)strlen(fecha) < 10) {
        copiarString(out, "??" "/????");      
        return;
    }
    out[0] = fecha[3];
    out[1] = fecha[4];
    out[2] = '/';
    out[3] = fecha[6];
    out[4] = fecha[7];
    out[5] = fecha[8];
    out[6] = fecha[9];
    out[7] = '\0';
}

static void ordenarPorCantidadDesc(Registro* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].cantidad < arr[j+1].cantidad) {
                Registro tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

// Total de ventas y su monto por anio
void mostrarTotalVentasPorAnio(struct Pedido** pedidos, int cantPedidos) {
    double montoTotal = 0.0;
    Registro* porAnio = NULL;
    int cantAnios = 0;

    for (int i = 0; i < cantPedidos; i++) {
        montoTotal += pedidos[i]->totalPedido;

        char* anio = malloc(5 * sizeof(char));
        anioDeFecha(pedidos[i]->fecha, anio);

        int pos = buscarRegistro(porAnio, cantAnios, anio, NULL);
        if (pos == -1) {
            porAnio = (Registro*)realloc(porAnio, (cantAnios + 1) * sizeof(Registro));
            porAnio[cantAnios].clave = asignarString(anio);
            porAnio[cantAnios].descripcion[0] = '\0';
            porAnio[cantAnios].cantidad = 1;
            porAnio[cantAnios].montoTotal = pedidos[i]->totalPedido;
            cantAnios++;
        } else {
            porAnio[pos].cantidad += 1;
            porAnio[pos].montoTotal += pedidos[i]->totalPedido;
        }
    }

    printf("=== TOTAL DE VENTAS ===\n");
    printf("Pedidos totales: %d\n", cantPedidos);
    printf("Monto total:     $%.2f\n\n", montoTotal);

    printf("=== MONTO POR AÑO ===\n");
    printf("%-8s %-10s %-12s\n", "Año", "Pedidos", "Monto");
    printf("----------------------------------\n");
    for (int i = 0; i < cantAnios; i++) {
        printf("%-8s %-10d $%-11.2f\n",
               porAnio[i].clave, porAnio[i].cantidad, porAnio[i].montoTotal);
    }
    printf("\n");

    free(porAnio);
}

// Clientes con mas pedidos
void mostrarClientesConMasPedidos(struct Pedido** pedidos, int cantPedidos, int topN) {
    Registro* porCliente = NULL;
    int cantClientes = 0;

    for (int i = 0; i < cantPedidos; i++) {
        int pos = buscarRegistro(porCliente, cantClientes, pedidos[i]->cedulaCliente, NULL);
        if (pos == -1) {
            porCliente = (Registro*)realloc(porCliente, (cantClientes + 1) * sizeof(Registro));
            porCliente[cantClientes].clave = asignarString(pedidos[i]->cedulaCliente);
            porCliente[cantClientes].descripcion = asignarString(pedidos[i]->nombreCliente);
            porCliente[cantClientes].cantidad = 1;              
            porCliente[cantClientes].montoTotal = pedidos[i]->totalPedido;
            cantClientes++;
        } else {
            porCliente[pos].cantidad += 1;
            porCliente[pos].montoTotal += pedidos[i]->totalPedido;
        }
    }

    ordenarPorCantidadDesc(porCliente, cantClientes);

    printf("=== CLIENTES CON MAS PEDIDOS (TOP %d) ===\n", topN);
    printf("%-4s %-15s %-25s %-8s %-12s\n", "No.", "Cedula", "Nombre", "Pedidos", "Monto");
    printf("------------------------------------------------------------------\n");
    for (int i = 0; i < cantClientes && i < topN; i++) {
        printf("%-4d %-15s %-25s %-8d $%-11.2f\n",
               i+1,
               porCliente[i].clave,
               porCliente[i].descripcion,
               porCliente[i].cantidad,
               porCliente[i].montoTotal);
    }
    printf("\n");

    free(porCliente);
}

//Libros mas vendidos
void mostrarLibrosMasVendidos(struct Pedido** pedidos, int cantPedidos, const char* anio, int topN) {
    int usarFiltro = (anio != NULL && anio[0] != '\0');

    Registro* porLibro = NULL;
    int cantLibros = 0;

    for (int i = 0; i < cantPedidos; i++) {
        if (usarFiltro) {
            char* a = malloc(5 * sizeof(char));
            anioDeFecha(pedidos[i]->fecha, a);
            if (!compararString(a, (char*)anio)) continue;
        }

        for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
            struct DetallePedido* d = pedidos[i]->detalles[j];

            int pos = buscarRegistro(porLibro, cantLibros, d->codigoLibro, NULL);
            if (pos == -1) {
                porLibro = (Registro*)realloc(porLibro, (cantLibros + 1) * sizeof(Registro));
                porLibro[cantLibros].clave = asignarString(d->codigoLibro);
                porLibro[cantLibros].descripcion = asignarString(d->nombreLibro);
                porLibro[cantLibros].cantidad = d->cantidad;
                porLibro[cantLibros].montoTotal = d->subtotal;
                cantLibros++;
            } else {
                porLibro[pos].cantidad += d->cantidad;
                porLibro[pos].montoTotal += d->subtotal;
            }
        }
    }

    ordenarPorCantidadDesc(porLibro, cantLibros);

    printf("=== LIBROS MAS VENDIDOS (TOP %d)%s%s ===\n",
           topN, usarFiltro ? " - Año " : "", usarFiltro ? anio : "");
    printf("%-4s %-15s %-30s %-10s\n", "No.", "Codigo", "Titulo", "Cantidad");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < cantLibros && i < topN; i++) {
        printf("%-4d %-15s %-30s %-10d\n",
               i+1,
               porLibro[i].clave,
               porLibro[i].descripcion,
               porLibro[i].cantidad);
    }
    printf("\n");

    free(porLibro);
}

void mostrarVentasPorMesAnio(struct Pedido** pedidos, int cantPedidos) {
    Registro* porMesAnio = NULL;
    int totalMeses = 0;

    for (int i = 0; i < cantPedidos; i++) {
        char* mesAnio = malloc(8 * sizeof(char));
        mesAnioDeFecha(pedidos[i]->fecha, mesAnio);

        int ind = buscarRegistro(porMesAnio, totalMeses, mesAnio, NULL);
        if (ind == -1) {
            porMesAnio = (Registro*)realloc(porMesAnio, (totalMeses + 1) * sizeof(Registro));
            porMesAnio[totalMeses].clave = asignarString(mesAnio);
            porMesAnio[totalMeses].descripcion[0] = '\0';
            porMesAnio[totalMeses].cantidad = 1;                    
            porMesAnio[totalMeses].montoTotal = pedidos[i]->totalPedido; 
            totalMeses++;
        } else {
            porMesAnio[ind].cantidad += 1;
            porMesAnio[ind].montoTotal += pedidos[i]->totalPedido;
        }
    }

    printf("=== TOTAL DE VENTAS POR MES-ANIO ===\n");
    printf("%-10s %-10s %-12s\n", "Mes/Anio", "Pedidos", "Monto");
    printf("------------------------------------\n");
    for (int i = 0; i < totalMeses; i++) {
        printf("%-10s %-10d $%-11.2f\n",
               porMesAnio[i].clave, porMesAnio[i].cantidad, porMesAnio[i].montoTotal);
    }
    printf("\n");

    free(porMesAnio);
}

void mostrarAutorTopPorAnio(struct Pedido** pedidos, int cantPedidos) {
    int cantLibros = 0;
    struct Libro** libros = cargarLibros(&cantLibros);

    if (libros == NULL || cantLibros == 0) {
        printf("No hay libros cargados, imposible calcular autores.\n\n");
        return;
    }

    Registro* porAutorAnio = NULL;
    int totalReg = 0;

    for (int i = 0; i < cantPedidos; i++) {
        char* anio = malloc(5 * sizeof(char));
        anioDeFecha(pedidos[i]->fecha, anio);

        for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
            struct DetallePedido* d = pedidos[i]->detalles[j];

            char* autor = NULL;
            for (int k = 0; k < cantLibros; k++) {
                if (compararString(libros[k]->codigo, d->codigoLibro)) {
                    autor = libros[k]->autor;
                    break;
                }
            }
            if (autor == NULL) continue; 

            int ind = buscarRegistro(porAutorAnio, totalReg, anio, autor);
            if (ind == -1) {
                porAutorAnio = (Registro*)realloc(porAutorAnio, (totalReg + 1) * sizeof(Registro));
                porAutorAnio[totalReg].clave = asignarString(anio);
                porAutorAnio[totalReg].descripcion = asignarString(autor);
                porAutorAnio[totalReg].cantidad = d->cantidad;      
                porAutorAnio[totalReg].montoTotal = d->subtotal;    
                totalReg++;
            } else {
                porAutorAnio[ind].cantidad += d->cantidad;
                porAutorAnio[ind].montoTotal += d->subtotal;
            }
        }
    }

    Registro* anios = NULL;
    int cantAnios = 0;
    for (int i = 0; i < totalReg; i++) {
        int ind = buscarRegistro(anios, cantAnios, porAutorAnio[i].clave, NULL);
        if (ind == -1) {
            anios = (Registro*)realloc(anios, (cantAnios + 1) * sizeof(Registro));
            strcpy(anios[cantAnios].clave, porAutorAnio[i].clave);
            cantAnios++;
        }
    }

    printf("=== AUTOR CON MAS VENTAS POR ANIO (POR MONTO) ===\n");
    printf("%-8s %-25s %-10s %-12s\n", "Anio", "Autor", "Unidades", "Monto");
    printf("-------------------------------------------------\n");
    for (int a = 0; a < cantAnios; a++) {
        const char* anio = anios[a].clave;

        double mejorMonto = -1.0;
        int mejorUnidades = 0;
        char* mejorAutor = malloc(64 * sizeof(char));
        mejorAutor[0] = '\0';

        for (int i = 0; i < totalReg; i++) {
            if (compararString(porAutorAnio[i].clave, (char*)anio)) {
                if (porAutorAnio[i].montoTotal > mejorMonto) {
                    mejorMonto = porAutorAnio[i].montoTotal;
                    mejorUnidades = porAutorAnio[i].cantidad;
                    strcpy(mejorAutor, porAutorAnio[i].descripcion);
                }
            }
        }

        if (mejorAutor[0] != '\0') {
            printf("%-8s %-25s %-10d $%-11.2f\n",
                   anio, mejorAutor, mejorUnidades, mejorMonto);
        }
    }
    printf("\n");

    free(anios);
    free(porAutorAnio);
}


