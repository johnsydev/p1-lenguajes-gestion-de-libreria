#ifndef PEDIDO_H
#define PEDIDO_H

#define TAM_CODIGO_LIBRO 20
#define TAM_FECHA 15
#define TAM_ID_PEDIDO 10
#define IMPUESTO_VENTA 0.13  // 13% de impuesto

struct DetallePedido {
    char *codigoLibro; //TAM_CODIGO_LIBRO
    char *nombreLibro; //100
    float precio;
    int cantidad;
    float subtotal;
};

struct Pedido {
    char *idPedido; //TAM_ID_PEDIDO
    char *cedulaCliente; //TAMANO_CEDULA
    char *nombreCliente; //TAMANO_NOMBRE
    char *fecha; //TAM_FECHA
    struct DetallePedido** detalles;
    int cantidadDetalles;
    float subtotalPedido;
    float impuesto;
    float totalPedido;
};

#endif
