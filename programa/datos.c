#include "cliente.h"
#include "datos.h"
#include "libro.h"
#include "auxiliares.h"
#include "pedido.h"
#include "registro.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                               SECCIÓN CLIENTES                              */
/* -------------------------------------------------------------------------- */

/*
    Nombre: cargarClientes
    Entrada: puntero a int cant (salida por referencia)
    Salida: arreglo dinámico de punteros a struct Cliente (struct Cliente**)
    Objetivo:
        Leer CLIENTES_TXT y construir en memoria un arreglo de clientes.
        Cada línea del archivo tiene el formato "cedula;nombre;telefono".
*/
struct Cliente** cargarClientes(int* cant) {
    int cantidadLineas;
    char** clientesTxt = leerArchivo(CLIENTES_TXT, &cantidadLineas);
    if (clientesTxt == NULL) {
        *cant = 0;
        return NULL;
    }
    struct Cliente** clientes = malloc(cantidadLineas * sizeof(struct Cliente*));
    for (int i = 0; i < cantidadLineas; i++) {
        char** info = separarTexto(clientesTxt[i], ';', 3);
        struct Cliente* cliente = malloc(sizeof(struct Cliente));
        cliente->cedula = asignarString(info[0]);
        cliente->nombre = asignarString(info[1]);
        cliente->telefono = asignarString(info[2]);
        clientes[i] = cliente;
        for (int j = 0; j < 3; j++) free(info[j]);
        free(info);
        free(clientesTxt[i]);
    }
    *cant = cantidadLineas;
    free(clientesTxt);
    return clientes;
}

/*
    Nombre: existeCedula
    Entrada: arreglo de clientes, cantidad, cedula a buscar
    Salida: 1 si existe, 0 si no
    Objetivo:
        Verificar existencia de una cédula en el arreglo de clientes.
*/
int existeCedula(struct Cliente** clientes, int cantidad, char* cedula) {
    for (int i = 0; i < cantidad; i++) {
        if (compararString(clientes[i]->cedula, cedula)) {
            return 1;
        }
    }
    return 0;
}

/*
    Nombre: validarTelefono
    Entrada: string teléfono
    Salida: 1 si es válido (>=7 dígitos y solo números), 0 si no
    Objetivo:
        Validar formato básico de un teléfono numérico.
*/
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

int validarFecha(char *fecha) {
    if (strlen(fecha) != 10) return 0;
    if (fecha[2] != '/' || fecha[5] != '/') return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit(fecha[i])) return 0;
    }
    return 1;
}

/*
    Nombre: registrarCliente
    Entrada: arreglo de clientes (puntero), cliente a agregar, puntero a cantidad
    Salida: NUEVO puntero al arreglo de clientes (struct Cliente**)
    Objetivo:
        Registrar un cliente nuevo. Verifica duplicados por cédula, persiste en
        archivo y devuelve (posible) nuevo puntero si hubo realloc. El caller debe
        asignar el valor retornado al arreglo original.
*/
struct Cliente** registrarCliente(struct Cliente** listaClientes, struct Cliente* cliente, int* cantidadClientes) {
    for (int i = 0; i < *cantidadClientes; i++) {
        if (compararString(listaClientes[i]->cedula, cliente->cedula)) {
            printf("Error: Ya existe un cliente con esa cedula.\n\n");
            return listaClientes;
        }
    }

    struct Cliente** nuevo = (struct Cliente**)realloc(listaClientes, (*cantidadClientes + 1) * sizeof(struct Cliente*));
    if (nuevo == NULL) {
        printf("Error: Memoria insuficiente al agregar cliente.\n\n");
        return listaClientes; // devolvemos el puntero viejo sin cambios
    }
    nuevo[*cantidadClientes] = cliente;
    (*cantidadClientes)++;

    FILE *archivo = fopen(CLIENTES_TXT, "a");
    if (archivo == NULL) {
        perror("fopen CLIENTES_TXT (append)");
        return nuevo; 
    }

    if (*cantidadClientes == 1) {
        fprintf(archivo, "%s;%s;%s", cliente->cedula, cliente->nombre, cliente->telefono);
    } else {
        fprintf(archivo, "\n%s;%s;%s", cliente->cedula, cliente->nombre, cliente->telefono);
    }
    fclose(archivo);
    return nuevo;
}

/*
    Nombre: actualizarTodosClientes
    Entrada: arreglo de clientes y puntero a cantidad
    Salida: void
    Objetivo:
        Reescribir completamente CLIENTES_TXT con el contenido en memoria.
*/
void actualizarTodosClientes(struct Cliente** clientes, int* cantidadClientes) {
    FILE *archivo = fopen(CLIENTES_TXT, "w");
    if (!archivo) { perror("fopen CLIENTES_TXT (write)"); return; }
    for (int i = 0; i < *cantidadClientes; i++) {
        if (i == 0) {
            fprintf(archivo, "%s;%s;%s", clientes[i]->cedula, clientes[i]->nombre, clientes[i]->telefono);
        } else {
            fprintf(archivo, "\n%s;%s;%s", clientes[i]->cedula, clientes[i]->nombre, clientes[i]->telefono);
        }
    }
    fclose(archivo);
}

/*
    Nombre: buscarIndiceCliente
    Entrada: arreglo de clientes, cantidad, cédula
    Salida: índice del cliente o -1 si no se encuentra
    Objetivo:
        Ubicar el índice de un cliente dado su número de cédula.
*/
static int buscarIndiceCliente(struct Cliente** clientes, int cantClientes, const char* cedula) {
    for (int i = 0; i < cantClientes; i++) {
        if (compararString(clientes[i]->cedula, (char*)cedula)) return i;
    }
    return -1;
}

/*
    Nombre: tienePedidosCliente
    Entrada: arreglo de pedidos, cantidad de pedidos, cédula
    Salida: true si hay pedidos asociados, false si no
    Objetivo:
        Verificar si un cliente posee pedidos registrados.
*/
bool tienePedidosCliente(struct Pedido** pedidos, int cantPedidos, const char* cedula) {
    for (int i = 0; i < cantPedidos; i++) {
        if (compararString(pedidos[i]->cedulaCliente, (char*)cedula)) {
            return true;
        }
    }
    return false;
}

/*
    Nombre: eliminarCliente
    Entrada: triple puntero a clientes (para poder realloc), puntero a cantidad,
             arreglo de pedidos y su cantidad, cédula a eliminar
    Salida: true si se eliminó, false en caso contrario
    Objetivo:
        Eliminar un cliente sin pedidos asociados, compactar memoria y reescribir
        archivo de clientes.
*/
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

    free((*clientes)[ind]->cedula);
    free((*clientes)[ind]->nombre);
    free((*clientes)[ind]->telefono);
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

/* -------------------------------------------------------------------------- */
/*                                   AUX                                      */
/* -------------------------------------------------------------------------- */

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
        free(linea);
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
    free(linea);
    *cantidadLineas = cantidad;
    return lineas;
}

/*
    Nombre: verificarAdmin
    Entrada: usuario, contraseña, puntero a Configuracion
    Salida: true si coincide user/pass, false si no
    Objetivo:
        Validar credenciales de administrador contra la configuración cargada.
*/
bool verificarAdmin(char* usuario, char* contrasena, struct Configuracion* config) {
    if (!config) { 
        return false;
    }

    return (compararString(config->usuarioAdmin, usuario) && compararString(config->contrasenaAdmin, contrasena));
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
        if (texto[indiceTexto] == delimitador) {
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
    free(temp);
    return array;
}

/* -------------------------------------------------------------------------- */
/*                              GESTIÓN DE LIBROS                              */
/* -------------------------------------------------------------------------- */

/*
    Nombre: cargarLibros
    Entrada: puntero a int cant (salida por referencia)
    Salida: arreglo dinámico de punteros a struct Libro (struct Libro**)
    Objetivo:
        Leer "libros.txt" y construir el arreglo de libros en memoria.
*/
struct Libro** cargarLibros(int* cant) {
    int cantidadLineas;
    char** librosTxt = leerArchivo("libros.txt", &cantidadLineas);
    if (librosTxt == NULL) {
        *cant = 0;
        printf("Error al leer el archivo.\n");
        return NULL;
    }
    struct Libro** libros = malloc(cantidadLineas * sizeof(struct Libro*));
    
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
        free(librosTxt[i]);
    }
    free(librosTxt);
    *cant = cantidadLineas;
    return libros;
}

/*
    Nombre: registrarLibro
    Entrada: arreglo de libros (puntero), libro a agregar, cantidad actual
    Salida: true si persiste en archivo, false si falla o hay duplicado
    Objetivo:
        Registrar un libro nuevo en archivo (sin modificar el arreglo en memoria).
        Verifica código duplicado.
*/
bool registrarLibro(struct Libro** listaLibros, struct Libro* libro, int cantidadLibros) {
    for (int i = 0; i < cantidadLibros; i++) {
        if (compararString(listaLibros[i]->codigo, libro->codigo)) {
            printf("Error: Ya existe un libro con ese codigo.\n\n");
            return false;
        }
    }

    FILE *archivo = fopen("libros.txt", "a");
    if (archivo == NULL) {
        perror("fopen libros.txt (append)");
        return false;
    }
    if (cantidadLibros == 0) {
        fprintf(archivo, "%s;%s;%s;%.2f;%d", libro->codigo, libro->nombre, libro->autor, libro->precio, libro->cantidad);
    } else {
        fprintf(archivo, "\n%s;%s;%s;%.2f;%d", libro->codigo, libro->nombre, libro->autor, libro->precio, libro->cantidad);
    }
    fclose(archivo);

    return true;
}

/*
    Nombre: actualizarTodosLibros
    Entrada: arreglo de libros y puntero a cantidad
    Salida: void
    Objetivo:
        Reescribir completamente "libros.txt" con el stock y datos actuales.
*/
void actualizarTodosLibros(struct Libro** libros, int* cantidadLibros) {
    FILE *limpiar = fopen("libros.txt", "w");
    if (!limpiar) { 
        perror("fopen libros.txt (truncate)"); 
        return; 
    }
    fclose(limpiar);

    FILE *archivo = fopen("libros.txt", "a");
    if (!archivo) { 
        perror("fopen libros.txt (append after truncate)"); 
        return; 
    }

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

/*
    Nombre: modificarInventario
    Entrada: arreglo de libros, puntero a cantidad, nombre de archivo de movimientos
    Salida: true si se procesa y persiste, false si falla la lectura
    Objetivo:
        Aplicar movimientos de inventario desde un archivo "codigo;delta".
        No permite stock negativo. Luego reescribe "libros.txt".
*/
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
                if (count >= 0) {
                    libros[j]->cantidad = count;
                }
                else {
                    printf("Linea %d no procesada, el stock quedaria negativo.\n", i+1);
                }
            }
        }
        free(info[0]);
        free(info[1]);
        free(info);
        free(contenido[i]);
    }
    free(contenido);
    printf("\n");
    actualizarTodosLibros(libros, cantidadLibros);
    return true;
}

/*
    Nombre: tienePedidosLibro
    Entrada: arreglo de pedidos, cantidad de pedidos, código de libro
    Salida: true si el libro aparece en algún pedido, false si no
    Objetivo:
        Verificar si un libro está referenciado en pedidos (para evitar borrado).
*/
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

/*
    Nombre: buscarIndiceLibro
    Entrada: arreglo de libros, cantidad, código
    Salida: índice o -1
    Objetivo:
        Encontrar el índice de un libro por su código.
*/
static int buscarIndiceLibro(struct Libro** libros, int cantLibros, const char* codigo) {
    for (int i = 0; i < cantLibros; i++) {
        if (compararString(libros[i]->codigo, (char*)codigo)) {
            return i;
        }
    }
    return -1;
}

/*
    Nombre: eliminarLibro
    Entrada: triple puntero a libros (para poder realloc), puntero a cantidad,
             arreglo de pedidos y su cantidad, código de libro a eliminar
    Salida: true si se elimina, false si no
    Objetivo:
        Eliminar un libro que no aparezca en pedidos, compactar memoria y
        reescribir "libros.txt".
*/
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
    free((*libros)[ind]->codigo);
    free((*libros)[ind]->nombre);
    free((*libros)[ind]->autor);
    free((*libros)[ind]);
    for (int i = ind; i < (*cantLibros) - 1; i++) {
        (*libros)[i] = (*libros)[i + 1];
    }
    (*cantLibros)--;

    if (*cantLibros > 0) {
        void* nuevo = realloc(*libros, (*cantLibros) * sizeof(struct Libro*));
        if (nuevo != NULL) {
            *libros = (struct Libro**)nuevo;  // si falla, mantenemos puntero viejo (ya compactado)
        } else {
            free(*libros);
            *libros = NULL;
        }
    } 
    actualizarTodosLibros(*libros, cantLibros);  
    printf("Libro eliminado correctamente.\n\n");
    return true;
}

/* -------------------------------------------------------------------------- */
/*                                  PEDIDOS                                   */
/* -------------------------------------------------------------------------- */

/*
    Nombre: buscarLibroPorCodigo
    Entrada: arreglo de libros, cantidad, código
    Salida: puntero al libro o NULL
    Objetivo:
        Retornar el libro cuyo código coincida exactamente.
*/
struct Libro* buscarLibroPorCodigo(struct Libro** libros, int cantidad, char* codigo) {
    for (int i = 0; i < cantidad; i++) {
        if (compararString(libros[i]->codigo, codigo)) {
            return libros[i];
        }
    }
    return NULL;
}

/*
    Nombre: buscarClientePorCedula
    Entrada: arreglo de clientes, cantidad, cédula
    Salida: puntero al cliente o NULL
    Objetivo:
        Retornar el cliente cuya cédula coincida exactamente.
*/
struct Cliente* buscarClientePorCedula(struct Cliente** clientes, int cantidad, char* cedula) {
    for (int i = 0; i < cantidad; i++) {
        if (compararString(clientes[i]->cedula, cedula)) {
            return clientes[i];
        }
    }
    return NULL;
}

/*
    Nombre: agregarDetallePedido
    Entrada: triple puntero a detalles (puede realloc), puntero a cantidad,
             código de libro, cantidad solicitada, arreglo de libros y su cantidad
    Salida: true si agrega/actualiza, false si hay error (stock o código)
    Objetivo:
        Agregar un renglón al detalle del pedido o actualizar cantidad si ya existe,
        validando stock disponible.
*/
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

/*
    Nombre: agregarDetallePedidoModificacion
    Entrada: triple puntero a detalles (puede realloc), puntero a cantidad,
             código de libro, cantidad solicitada, arreglo de libros y su cantidad, doble púntero a detalles originales y su cantidad
    Salida: true si agrega/actualiza, false si hay error (stock o código)
    Objetivo:
        Agregar un renglón al detalle del pedido o actualizar cantidad si ya existe,
        validando stock disponible. En este caso, se considera el stock original del pedido
        para permitir que el usuario pueda volver a agregar un libro que ya estaba en el pedido.
*/
bool agregarDetallePedidoModificacion(struct DetallePedido*** detalles, int* cantidadDetalles, 
                                     char* codigoLibro, int cantidad, struct Libro** libros, 
                                     int cantLibros, struct DetallePedido** detallesOriginales, 
                                     int cantDetallesOriginales) {
    struct Libro* libro = buscarLibroPorCodigo(libros, cantLibros, codigoLibro);
    if (libro == NULL) {
        printf("Libro no encontrado.\n\n");
        return false;
    }
    
    // Calcular stock disponible considerando el pedido original
    int stockDisponible = libro->cantidad;
    
    // Sumar la cantidad original de este libro si estaba en el pedido
    for (int i = 0; i < cantDetallesOriginales; i++) {
        if (compararString(detallesOriginales[i]->codigoLibro, codigoLibro)) {
            stockDisponible += detallesOriginales[i]->cantidad;
            break;
        }
    }
    
    // Restar la cantidad actual de este libro en el pedido modificado
    for (int i = 0; i < *cantidadDetalles; i++) {
        if (compararString((*detalles)[i]->codigoLibro, codigoLibro)) {
            stockDisponible -= (*detalles)[i]->cantidad;
        }
    }
    
    if (stockDisponible < cantidad) {
        printf("Stock insuficiente. Disponible: %d\n\n", stockDisponible);
        return false;
    }

    // Verificar si el libro ya existe en el pedido actual
    for (int i = 0; i < *cantidadDetalles; i++) {
        if (compararString((*detalles)[i]->codigoLibro, codigoLibro)) {
            printf("El libro ya está en el pedido. Use 'Cambiar cantidad' en su lugar.\n\n");
            return false;
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

/*
    Nombre: eliminarDetallePedido
    Entrada: triple puntero a detalles, puntero a cantidad de detalles, número de línea (1-based)
    Salida: true si elimina, false si índice inválido
    Objetivo:
        Eliminar una línea del detalle de pedido, compactar el arreglo y ajustar tamaño.
*/
bool eliminarDetallePedido(struct DetallePedido*** detalles, int* cantidadDetalles, int numeroLinea) {
    if (numeroLinea < 1 || numeroLinea > *cantidadDetalles) {
        printf("Error: Numero de linea invalido.\n\n");
        return false;
    }
    
    int indice = numeroLinea - 1;
    free((*detalles)[indice]->codigoLibro);
    free((*detalles)[indice]->nombreLibro);
    free((*detalles)[indice]);
    
    for (int i = indice; i < *cantidadDetalles - 1; i++) {
        (*detalles)[i] = (*detalles)[i + 1];
    }
    
    (*cantidadDetalles)--;
    *detalles = realloc(*detalles, (*cantidadDetalles) * sizeof(struct DetallePedido*));
    
    printf("Linea eliminada correctamente.\n\n");
    return true;
}

/*
    Nombre: calcularTotalesPedido
    Entrada: arreglo de detalles, cantidad de detalles, punteros a subtotal, impuesto y total
    Salida: llena los valores por referencia
    Objetivo:
        Calcular subtotal (suma de subtotales), impuesto (por IMPUESTO_VENTA) y total.
*/
void calcularTotalesPedido(struct DetallePedido** detalles, int cantidadDetalles, float* subtotal, float* impuesto, float* total) {
    *subtotal = 0;
    for (int i = 0; i < cantidadDetalles; i++) {
        *subtotal += detalles[i]->subtotal;
    }
    *impuesto = *subtotal * IMPUESTO_VENTA;
    *total = *subtotal + *impuesto;
}

/*
    Nombre: eliminarPedido
    Entrada: triple puntero a pedidos (para poder realloc), puntero a cantidad,
             id de pedido a eliminar
    Salida: true si elimina y reescribe archivos, false si falla
    Objetivo:
        Eliminar un pedido, restaurar stock de libros, compactar memoria y
        reescribir PEDIDOS_TXT y DETALLES_TXT.
*/
bool eliminarPedido(struct Pedido*** pedidos, int* cantidadPedidos, const char* idPedido) {
    if (pedidos == NULL || *pedidos == NULL || *cantidadPedidos <= 0) {
        printf("No hay pedidos cargados.\n\n");
        return false;
    }

    int ind = -1;
    for (int i = 0; i < *cantidadPedidos; i++) {
        if (compararString((*pedidos)[i]->idPedido, (char*)idPedido)) {
            ind = i;
            break;
        }
    }
    if (ind < 0) {
        printf("Pedido no encontrado.\n\n");
        return false;
    }

    // Revertir el stock de los libros ANTES de eliminar el pedido
    int cantLibros = 0;
    struct Libro** libros = cargarLibros(&cantLibros);
    
    if (libros != NULL) {
        for (int i = 0; i < (*pedidos)[ind]->cantidadDetalles; i++) {
            struct DetallePedido* detalle = (*pedidos)[ind]->detalles[i];
            struct Libro* libro = buscarLibroPorCodigo(libros, cantLibros, detalle->codigoLibro);
            if (libro != NULL) {
                libro->cantidad += detalle->cantidad;
                printf("Stock restaurado para libro %s: +%d unidades\n", 
                       detalle->codigoLibro, detalle->cantidad);
            }
        }
        // Actualizar archivo de libros con el stock restaurado
        actualizarTodosLibros(libros, &cantLibros);
        
        // Liberar memoria de libros
        for (int i = 0; i < cantLibros; i++) {
            free(libros[i]->codigo);
            free(libros[i]->nombre);
            free(libros[i]->autor);
            free(libros[i]);
        }
        free(libros);
    }

    // Liberar memoria del pedido que se va a eliminar
    for (int i = 0; i < (*pedidos)[ind]->cantidadDetalles; i++) {
        free((*pedidos)[ind]->detalles[i]->codigoLibro);
        free((*pedidos)[ind]->detalles[i]->nombreLibro);
        free((*pedidos)[ind]->detalles[i]);
    }
    free((*pedidos)[ind]->detalles);
    free((*pedidos)[ind]->idPedido);
    free((*pedidos)[ind]->cedulaCliente);
    free((*pedidos)[ind]->nombreCliente);
    free((*pedidos)[ind]->fecha);
    free((*pedidos)[ind]);

    
    for (int i = ind; i < (*cantidadPedidos) - 1; i++) {
        (*pedidos)[i] = (*pedidos)[i + 1];
    }
    (*cantidadPedidos)--;

    
    if (*cantidadPedidos > 0) {
        void* nuevo = realloc(*pedidos, (*cantidadPedidos) * sizeof(struct Pedido*));
        if (nuevo != NULL) *pedidos = (struct Pedido**)nuevo; 
    } else {
        free(*pedidos);
        *pedidos = NULL;
    }

    // Usar la nueva función para reescribir archivos
    reescribirArchivosPedidos(*pedidos, *cantidadPedidos);

    printf("Pedido eliminado correctamente y stock restaurado.\n\n");
    return true;
}

/*
    Nombre: mostrarDetallePedido
    Entrada: arreglo de detalles, cantidad
    Salida: void
    Objetivo:
        Imprimir en consola el detalle (líneas) de un pedido.
*/
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

/*
    Nombre: generarPedido
    Entrada: puntero a Pedido (completo), arreglo de libros y su cantidad,
             puntero a Configuracion
    Salida: true si persiste encabezado y detalles, false si falla
    Objetivo:
        Generar un ID secuencial, descontar stock, actualizar "libros.txt" y
        guardar encabezado en PEDIDOS_TXT y renglones en DETALLES_TXT.
*/
bool generarPedido(struct Pedido* pedido, struct Libro** libros, int* cantLibros, struct Configuracion* config) {
    // Generar ID secuencial
    char* idGenerado = generarIdPedidoSecuencial(config);
    pedido->idPedido = asignarString(idGenerado);
    free(idGenerado);
    
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

/*
    Nombre: mostrarPedidoCompleto
    Entrada: puntero a Pedido y puntero a Configuracion
    Salida: void
    Objetivo:
        Imprimir encabezado del pedido, líneas y totales con los datos del local.
*/
void mostrarPedidoCompleto(struct Pedido* pedido, struct Configuracion* config) {
    printf("\n");
    printf("===============================================\n");
    printf("                %s\n", config ? config->nombreLocal : "COMERCIO");
    printf("           Tel: %s\n", config ? config->telefono : "N/A");
    printf("        Cédula: %s\n", config ? config->cedulaJuridica : "N/A");
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
    printf("===============================================\n");
    if (config && config->horarioAtencion) {
        printf("Horario: %s\n", config->horarioAtencion);
    }
    printf("===============================================\n\n");
}

/*
    Nombre: cargarPedidos
    Entrada: puntero a int cant (salida por referencia)
    Salida: arreglo de punteros a Pedido
    Objetivo:
        Leer PEDIDOS_TXT y construir cada pedido con sus detalles desde archivo.
*/
struct Pedido** cargarPedidos(int* cant) {
    *cant = 0;
    int cantidadLineas = 0;
    char** lineas = leerArchivo(PEDIDOS_TXT, &cantidadLineas);
    
    if (lineas == NULL || cantidadLineas == 0) {
        return NULL;
    }

    struct Pedido** pedidos = malloc(cantidadLineas * sizeof(struct Pedido*));
    int contador = 0;
    
    for (int i = 0; i < cantidadLineas; i++) {
        if (lineas[i] == NULL || strlen(lineas[i]) == 0) continue;
        
        char** parts = separarTexto(lineas[i], ';', 7);
        if (parts == NULL) continue;

        pedidos[contador] = malloc(sizeof(struct Pedido));
        
        pedidos[contador]->idPedido = asignarString(parts[0]);
        pedidos[contador]->cedulaCliente = asignarString(parts[1]);
        pedidos[contador]->nombreCliente = asignarString(parts[2]);
        pedidos[contador]->fecha = asignarString(parts[3]);
        pedidos[contador]->subtotalPedido = atof(parts[4]);
        pedidos[contador]->impuesto = atof(parts[5]);
        pedidos[contador]->totalPedido = atof(parts[6]);

        // Cargar detalles para este pedido
        int cantDetalles = 0;
        struct DetallePedido** detalles = cargarDetallesPorPedido(pedidos[contador]->idPedido, &cantDetalles);
        pedidos[contador]->detalles = detalles;
        pedidos[contador]->cantidadDetalles = cantDetalles;

        contador++;

        // Liberar parts
        for (int p = 0; p < 7; p++) {
            if (parts[p]) free(parts[p]);
        }
        free(parts);
    }

    // Liberar lineas
    for (int i = 0; i < cantidadLineas; i++) {
        if (lineas[i]) free(lineas[i]);
    }
    free(lineas);

    if (contador == 0) {
        free(pedidos);
        return NULL;
    }

    // Ajustar tamaño exacto
    void* nuevo = realloc(pedidos, contador * sizeof(struct Pedido*));
    if (nuevo) pedidos = (struct Pedido**)nuevo;
    
    *cant = contador;
    return pedidos;
}

/*
    Nombre: cargarDetallesPorPedido
    Entrada: id de pedido, puntero a int cant (salida por referencia)
    Salida: arreglo de punteros a DetallePedido
    Objetivo:
        Leer DETALLES_TXT y retornar únicamente los detalles pertenecientes al id dado.
*/
struct DetallePedido** cargarDetallesPorPedido(char* idPedido, int* cant) {
    *cant = 0;
    int cantidadLineas = 0;
    char** lineas = leerArchivo(DETALLES_TXT, &cantidadLineas);
    
    if (lineas == NULL || cantidadLineas == 0) {
        return NULL;
    }

    struct DetallePedido** detalles = NULL;
    int contador = 0;
    
    for (int i = 0; i < cantidadLineas; i++) {
        if (lineas[i] == NULL || strlen(lineas[i]) == 0) continue;
        
        char** parts = separarTexto(lineas[i], ';', 6);
        if (parts == NULL) continue;
        
        if (compararString(parts[0], idPedido)) {
            detalles = realloc(detalles, (contador + 1) * sizeof(struct DetallePedido*));
            detalles[contador] = malloc(sizeof(struct DetallePedido));
            
            detalles[contador]->codigoLibro = asignarString(parts[1]);
            detalles[contador]->nombreLibro = asignarString(parts[2]);
            detalles[contador]->precio = atof(parts[3]);
            detalles[contador]->cantidad = atoi(parts[4]);
            detalles[contador]->subtotal = atof(parts[5]);
            
            contador++;
        }
        
        // Liberar parts
        for (int p = 0; p < 6; p++) {
            if (parts[p]) free(parts[p]);
        }
        free(parts);
    }

    // Liberar lineas
    for (int i = 0; i < cantidadLineas; i++) {
        if (lineas[i]) free(lineas[i]);
    }
    free(lineas);

    *cant = contador;
    return detalles;
}

/*
    Nombre: buscarIndicePedido
    Entrada: arreglo de pedidos, cantidad, id a buscar
    Salida: índice o -1
    Objetivo:
        Devolver el índice de un pedido según su id.
*/
static int buscarIndicePedido(struct Pedido** pedidos, int cant, const char* id) {
    for (int i = 0; i < cant; i++) {
        if (compararString(pedidos[i]->idPedido, (char*)id)) {
            return i;
        }
    }
    return -1;
}

/*
    Nombre: reescribirArchivosPedidos
    Entrada: arreglo de pedidos en memoria, cantidad
    Salida: true si ambos archivos se reescriben, false si ocurre un error
    Objetivo:
        Reescribir por completo PEDIDOS_TXT y DETALLES_TXT con el contenido
        actual del arreglo en memoria (mismo enfoque que eliminarPedido()).
*/
bool reescribirArchivosPedidos(struct Pedido** pedidos, int cantidadPedidos) {
    if (pedidos == NULL || cantidadPedidos == 0) {
        // Vaciar archivos
        FILE *archivoPedidos = fopen(PEDIDOS_TXT, "w");
        if (archivoPedidos) fclose(archivoPedidos);
        
        FILE *archivoDetalles = fopen(DETALLES_TXT, "w");
        if (archivoDetalles) fclose(archivoDetalles);
        
        return true;
    }

    // Reescribir archivo de pedidos
    FILE *archivoPedidos = fopen(PEDIDOS_TXT, "w");
    if (archivoPedidos == NULL) {
        printf("Error al reescribir archivo de pedidos.\n");
        return false;
    }
    
    for (int i = 0; i < cantidadPedidos; i++) {
        if (i == 0) {
            fprintf(archivoPedidos, "%s;%s;%s;%s;%.2f;%.2f;%.2f", 
                    pedidos[i]->idPedido, pedidos[i]->cedulaCliente, 
                    pedidos[i]->nombreCliente, pedidos[i]->fecha, 
                    pedidos[i]->subtotalPedido, pedidos[i]->impuesto, 
                    pedidos[i]->totalPedido);
        } else {
            fprintf(archivoPedidos, "\n%s;%s;%s;%s;%.2f;%.2f;%.2f", 
                    pedidos[i]->idPedido, pedidos[i]->cedulaCliente, 
                    pedidos[i]->nombreCliente, pedidos[i]->fecha, 
                    pedidos[i]->subtotalPedido, pedidos[i]->impuesto, 
                    pedidos[i]->totalPedido);
        }
    }
    fclose(archivoPedidos);

    // Reescribir archivo de detalles
    FILE *archivoDetalles = fopen(DETALLES_TXT, "w");
    if (archivoDetalles == NULL) {
        printf("Error al reescribir archivo de detalles.\n");
        return false;
    }
    
    bool primero = true;
    for (int i = 0; i < cantidadPedidos; i++) {
        for (int j = 0; j < pedidos[i]->cantidadDetalles; j++) {
            if (primero) {
                fprintf(archivoDetalles, "%s;%s;%s;%.2f;%d;%.2f",
                        pedidos[i]->idPedido, pedidos[i]->detalles[j]->codigoLibro, 
                        pedidos[i]->detalles[j]->nombreLibro, pedidos[i]->detalles[j]->precio, 
                        pedidos[i]->detalles[j]->cantidad, pedidos[i]->detalles[j]->subtotal);
                primero = false;
            } else {
                fprintf(archivoDetalles, "\n%s;%s;%s;%.2f;%d;%.2f",
                        pedidos[i]->idPedido, pedidos[i]->detalles[j]->codigoLibro, 
                        pedidos[i]->detalles[j]->nombreLibro, pedidos[i]->detalles[j]->precio, 
                        pedidos[i]->detalles[j]->cantidad, pedidos[i]->detalles[j]->subtotal);
            }
        }
    }
    fclose(archivoDetalles);
    
    return true;
}

/* -------------------------------------------------------------------------- */
/*                                ESTADÍSTICAS                                */
/* -------------------------------------------------------------------------- */

/*
    Nombre: buscarRegistro
    Entrada: arreglo de Registro, cantidad, clave, descripcionOpcional (puede ser NULL)
    Salida: índice del registro que coincide o -1
    Objetivo:
        Ubicar un registro por clave y, opcionalmente, por descripción.
*/
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

/*
    Nombre: anioDeFecha
    Entrada: fecha (string), buffer anio (char[5])
    Salida: escribe el año en 'anio'
    Objetivo:
        Tomar los últimos 4 caracteres de la fecha como año.
*/
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

/*
    Nombre: mesAnioDeFecha
    Entrada: fecha (string), buffer out (char[8])
    Salida: escribe "MM/YYYY" en 'out'
    Objetivo:
        Extraer mes y año de una fecha esperada como dd/mm/aaaa.
*/
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

/*
    Nombre: ordenarPorCantidadDesc
    Entrada: arreglo de Registro, cantidad
    Salida: void
    Objetivo:
        Ordenar el arreglo de mayor a menor por el campo 'cantidad'.
*/
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

/*
    Nombre: mostrarTotalVentasPorAnio
    Entrada: arreglo de pedidos, cantidad de pedidos
    Salida: void
    Objetivo:
        Imprimir cantidad y monto total de pedidos, y el desglose por año.
*/
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
            porAnio[cantAnios].descripcion = asignarString(""); 
            porAnio[cantAnios].cantidad = 1;
            porAnio[cantAnios].montoTotal = pedidos[i]->totalPedido;
            cantAnios++;
        } else {
            porAnio[pos].cantidad += 1;
            porAnio[pos].montoTotal += pedidos[i]->totalPedido;
        }
        free(anio);
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

    for (int i = 0; i < cantAnios; i++) {
        free(porAnio[i].clave);
        if (porAnio[i].descripcion) free(porAnio[i].descripcion);
    }
    free(porAnio);
}

/*
    Nombre: mostrarClientesConMasPedidos
    Entrada: arreglo de pedidos, cantidad, topN
    Salida: void
    Objetivo:
        Mostrar el Top-N de clientes por cantidad de pedidos y monto acumulado.
*/
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

    for (int i = 0; i < cantClientes; i++) {
        free(porCliente[i].clave);
        if (porCliente[i].descripcion) free(porCliente[i].descripcion);
    }
    free(porCliente);
}

/*
    Nombre: mostrarLibrosMasVendidos
    Entrada: arreglo de pedidos, cantidad, filtro de año (puede NULL o ""), topN
    Salida: void
    Objetivo:
        Contar unidades por código de libro (y año opcional) y mostrar Top-N.
*/
void mostrarLibrosMasVendidos(struct Pedido** pedidos, int cantPedidos, const char* anio, int topN) {
    int usarFiltro = (anio != NULL && anio[0] != '\0');

    Registro* porLibro = NULL;
    int cantLibros = 0;

    for (int i = 0; i < cantPedidos; i++) {
        if (usarFiltro) {
            char* a = malloc(5 * sizeof(char));
            anioDeFecha(pedidos[i]->fecha, a);
            if (!compararString(a, (char*)anio)) {
                free(a); continue;
            }
            free(a);
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

    for (int i = 0; i < cantLibros; i++) {
        free(porLibro[i].clave);
        if (porLibro[i].descripcion) free(porLibro[i].descripcion);
    }
    free(porLibro);
}

/*
    Nombre: mostrarVentasPorMesAnio
    Entrada: arreglo de pedidos, cantidad
    Salida: void
    Objetivo:
        Mostrar cantidad de pedidos y monto total agrupado por "MM/YYYY".
*/
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
            porMesAnio[totalMeses].descripcion = asignarString("");
            porMesAnio[totalMeses].cantidad = 1;                    
            porMesAnio[totalMeses].montoTotal = pedidos[i]->totalPedido; 
            totalMeses++;
        } else {
            porMesAnio[ind].cantidad += 1;
            porMesAnio[ind].montoTotal += pedidos[i]->totalPedido;
        }
        free(mesAnio);
    }

    printf("=== TOTAL DE VENTAS POR MES-ANIO ===\n");
    printf("%-10s %-10s %-12s\n", "Mes/Anio", "Pedidos", "Monto");
    printf("------------------------------------\n");
    for (int i = 0; i < totalMeses; i++) {
        printf("%-10s %-10d $%-11.2f\n",
               porMesAnio[i].clave, porMesAnio[i].cantidad, porMesAnio[i].montoTotal);
    }
    printf("\n");

    for (int i = 0; i < totalMeses; i++) {
        free(porMesAnio[i].clave);
        if (porMesAnio[i].descripcion) free(porMesAnio[i].descripcion);
    }
    free(porMesAnio);

}

/*
    Nombre: mostrarAutorTopPorAnio
    Entrada: arreglo de pedidos, cantidad
    Salida: void
    Objetivo:
        Determinar por cada año el autor con mayor monto vendido (y mostrar sus
        unidades y monto).
*/
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
        free(anio);
    }

    Registro* anios = NULL;
    int cantAnios = 0;
    for (int i = 0; i < totalReg; i++) {
        int ind = buscarRegistro(anios, cantAnios, porAutorAnio[i].clave, NULL);
        if (ind == -1) {
            anios = (Registro*)realloc(anios, (cantAnios + 1) * sizeof(Registro));
            anios[cantAnios].clave = asignarString(porAutorAnio[i].clave);  
            anios[cantAnios].descripcion = asignarString("");               
            anios[cantAnios].cantidad = 0;
            anios[cantAnios].montoTotal = 0.0;
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
                    copiarString(mejorAutor, porAutorAnio[i].descripcion);
                }
            }
        }

        if (mejorAutor[0] != '\0') {
            printf("%-8s %-25s %-10d $%-11.2f\n",
                   anio, mejorAutor, mejorUnidades, mejorMonto);
        }
        free(mejorAutor);
    }
    printf("\n");

    for (int i = 0; i < cantAnios; i++) {
        free(anios[i].clave);
        if (anios[i].descripcion) free(anios[i].descripcion);
    }
    free(anios);

    for (int i = 0; i < totalReg; i++) {
        free(porAutorAnio[i].clave);
        if (porAutorAnio[i].descripcion) free(porAutorAnio[i].descripcion);
    }
    free(porAutorAnio);

    for (int i = 0; i < cantLibros; i++) {
        free(libros[i]->codigo);
        free(libros[i]->nombre);
        free(libros[i]->autor);
        free(libros[i]);
    }
    free(libros);

}
