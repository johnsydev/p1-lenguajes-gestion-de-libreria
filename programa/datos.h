#ifndef DATOS_H
#define DATOS_H

#include <stdbool.h>

#include "libro.h"
#include "cliente.h"
#include "pedido.h"

#define CLIENTES_TXT "clientes.txt"
#define PEDIDOS_TXT "pedidos.txt"
#define DETALLES_TXT "detalles.txt"

char** leerArchivo(char*, int*);
bool verificarAdmin(char*, char*);
char** separarTexto(char*, char, int);

// Libros
bool registrarLibro(struct Libro**, struct Libro*, int);
struct Libro** cargarLibros(int*);
void actualizarTodosLibros(struct Libro**, int*);
bool modificarInventario(struct Libro**, int*, char*);
bool tienePedidosLibro(struct Pedido** pedidos, int cantPedidos, const char* codigoLibro);
bool eliminarLibro(struct Libro*** libros, int* cantLibros, struct Pedido** pedidos, int cantPedidos, const char* codigoLibro);

// Clientes
struct Cliente** cargarClientes(int*);
bool registrarCliente(struct Cliente**, struct Cliente*, int);
void actualizarTodosClientes(struct Cliente**, int*);
int validarTelefono(char* telefono);
int existeCedula(struct Cliente**, int, char*);
bool tienePedidosCliente(struct Pedido** pedidos, int cantPedidos, const char* cedula);
bool eliminarCliente(struct Cliente*** clientes, int* cantClientes, struct Pedido** pedidos, int cantPedidos, const char* cedula);

// Pedidos
struct Libro* buscarLibroPorCodigo(struct Libro**, int, char*);
struct Cliente* buscarClientePorCedula(struct Cliente**, int, char*);
bool agregarDetallePedido(struct DetallePedido***, int*, char*, int, struct Libro**, int);
bool eliminarDetallePedido(struct DetallePedido***, int*, int);
void calcularTotalesPedido(struct DetallePedido**, int, float*, float*, float*);
bool generarPedido(struct Pedido*, struct Libro**, int*);
char* generarIdPedido();
void mostrarDetallePedido(struct DetallePedido**, int);
void mostrarPedidoCompleto(struct Pedido*);
struct Pedido** cargarPedidos(int*);
struct DetallePedido** cargarDetallesPorPedido(char*, int*);

// Estadisticas 
void mostrarTotalVentasPorAnio(struct Pedido** pedidos, int cantPedidos);
void mostrarClientesConMasPedidos(struct Pedido** pedidos, int cantPedidos, int topN);
void mostrarLibrosMasVendidos(struct Pedido** pedidos, int cantPedidos, const char* anio, int topN);
void mostrarVentasPorMesAnio(struct Pedido** pedidos, int cantPedidos);
void mostrarAutorTopPorAnio(struct Pedido** pedidos, int cantPedidos);
#endif


