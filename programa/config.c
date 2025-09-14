#include "config.h"
#include "auxiliares.h"
#include "pedido.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILE "config.json"

// Función auxiliar para extraer valor JSON simple
static char* extraerValorJSON(const char* linea, const char* clave) {
    char* inicio = strstr(linea, clave);
    if (!inicio) {
        return NULL;
    }

    inicio = strchr(inicio, ':');
    if (!inicio) {
        return NULL;
    }
    inicio++;
    
    // Buscar el primer "
    while (*inicio == ' ' || *inicio == '\t') { 
        inicio++; 
    }

    if (*inicio != '"') {return NULL;
        return NULL;
    }
    inicio++;
    
    // Buscar el último "
    char* fin = strchr(inicio, '"');
    if (!fin) {
        return NULL;
    }

    int longitud = fin - inicio;
    char* valor = malloc((longitud + 1) * sizeof(char));
    strncpy(valor, inicio, longitud);
    valor[longitud] = '\0';
    
    return valor;
}

static int extraerEnteroJSON(const char* linea, const char* clave) {
    char* inicio = strstr(linea, clave);
    if (!inicio) {
        return 0;
    }
    
    inicio = strchr(inicio, ':');
    if (!inicio) {
        return 0;
    }
    inicio++;

    while (*inicio == ' ' || *inicio == '\t') {
        inicio++;
    }
    return atoi(inicio);
}

struct Configuracion* cargarConfiguracion() {
    FILE* archivo = fopen(CONFIG_FILE, "r");
    if (!archivo) {
        printf("Error: No se puede abrir el archivo de configuración.\n");
        return NULL;
    }
    
    struct Configuracion* config = malloc(sizeof(struct Configuracion));
    config->nombreLocal = NULL;
    config->telefono = NULL;
    config->cedulaJuridica = NULL;
    config->horarioAtencion = NULL;
    config->secuencialPedido = 1001;
    config->usuarioAdmin = NULL;
    config->contrasenaAdmin = NULL;

    char* linea = malloc(500 * sizeof(char));
    while (fgets(linea, 500, archivo)) {
        char* valor;
        
        if ((valor = extraerValorJSON(linea, "nombreLocal")) != NULL) {
            config->nombreLocal = valor;
        }
        else if ((valor = extraerValorJSON(linea, "telefono")) != NULL) {
            config->telefono = valor;
        }
        else if ((valor = extraerValorJSON(linea, "cedulaJuridica")) != NULL) {
            config->cedulaJuridica = valor;
        }
        else if ((valor = extraerValorJSON(linea, "horarioAtencion")) != NULL) {
            config->horarioAtencion = valor;
        }
        else if ((valor = extraerValorJSON(linea, "usuarioAdmin")) != NULL) {
            config->usuarioAdmin = valor;
        }
        else if ((valor = extraerValorJSON(linea, "contrasenaAdmin")) != NULL) {
            config->contrasenaAdmin = valor;
        }
        else if (strstr(linea, "secuencialPedido")) {
            config->secuencialPedido = extraerEnteroJSON(linea, "secuencialPedido");
        }
    }
    
    fclose(archivo);
    return config;
}

void liberarConfiguracion(struct Configuracion* config) {
    if (!config){
        return;
    }

    free(config->nombreLocal);
    free(config->telefono);
    free(config->cedulaJuridica);
    free(config->horarioAtencion);
    free(config->usuarioAdmin);
    free(config->contrasenaAdmin);
    free(config);
}

void guardarConfiguracion(struct Configuracion* config) {
    if (!config) {
        return;
    }
    
    FILE* archivo = fopen(CONFIG_FILE, "w");
    if (!archivo) {
        printf("Error: No se puede guardar la configuración.\n");
        return;
    }
    
    fprintf(archivo, "{\n");
    fprintf(archivo, "    \"nombreLocal\": \"%s\",\n", config->nombreLocal ? config->nombreLocal : "");
    fprintf(archivo, "    \"telefono\": \"%s\",\n", config->telefono ? config->telefono : "");
    fprintf(archivo, "    \"cedulaJuridica\": \"%s\",\n", config->cedulaJuridica ? config->cedulaJuridica : "");
    fprintf(archivo, "    \"horarioAtencion\": \"%s\",\n", config->horarioAtencion ? config->horarioAtencion : "");
    fprintf(archivo, "    \"secuencialPedido\": %d,\n", config->secuencialPedido);
    fprintf(archivo, "    \"usuarioAdmin\": \"%s\",\n", config->usuarioAdmin ? config->usuarioAdmin : "");
    fprintf(archivo, "    \"contrasenaAdmin\": \"%s\"\n", config->contrasenaAdmin ? config->contrasenaAdmin : "");
    fprintf(archivo, "}\n");
    
    fclose(archivo);
}

char* generarIdPedidoSecuencial(struct Configuracion* config) {
    char* id = malloc(TAM_ID_PEDIDO * sizeof(char));
    snprintf(id, TAM_ID_PEDIDO, "%d", config->secuencialPedido);
    config->secuencialPedido++;
    guardarConfiguracion(config);
    return id;
}