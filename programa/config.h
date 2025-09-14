#ifndef CONFIG_H
#define CONFIG_H

struct Configuracion {
    char* nombreLocal;
    char* telefono;
    char* cedulaJuridica;
    char* horarioAtencion;
    int secuencialPedido;
    char* usuarioAdmin;
    char* contrasenaAdmin;
};

struct Configuracion* cargarConfiguracion();
void liberarConfiguracion(struct Configuracion* config);
void guardarConfiguracion(struct Configuracion* config);
char* generarIdPedidoSecuencial(struct Configuracion* config);

#endif