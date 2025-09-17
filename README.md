# Gestión de Librería

Proyecto Programado #1 para el curso **Lenguajes de Programación**  
II Semestre, 2025  
Fecha de entrega: Martes 16 de septiembre del 2025

## Descripción

Este sistema permite la gestión de una librería, implementado en C, con almacenamiento de datos en archivos de texto. Permite registrar libros, clientes, realizar pedidos, y gestionar la información.

## Autores

- Johnsy Steven López Aguilar — **2024178835**
- David Calvo García — **2024122451**
- Erin Camacho González — **2024073710**

## Estructura del Proyecto

- **programa/**: Contiene el código fuente en C y archivos de datos.
  - `main.c`: ejecuta el menú principal.
  - `menu.c`: Lógica de interacción con el usuario y menús.
  - `datos.c`, `auxiliares.c`, `config.c`, etc.: Funciones para manejar datos, utilidades y configuración.
  - Archivos de datos (`.txt`):
    - `libros.txt`: Libros disponibles (`código;nombre;autor;precio;cantidad`).
    - `clientes.txt`: Clientes registrados (`cédula;nombre;teléfono`).
    - `pedidos.txt`: Pedidos realizados (`número;cedula;nombre;fecha;monto;impuesto;total`).
    - `detalles.txt`: Detalle de cada pedido (`número_pedido;código_libro;nombre_libro;precio;cantidad;subtotal`).
    - `CARGAR.txt`: Datos de carga de inventario.
  - `config.json`: Archivo de configuración.

- **info.txt**: Información general del proyecto, autores y estatus.

## Funcionalidad Principal

- Registro y consulta de libros, clientes y pedidos.
- Validación y carga de datos desde archivos planos.
- Menú interactivo para gestionar las operaciones principales de la librería.
- Estructuras dinámicas en C para manejo eficiente de datos.

## Ejecución

1. **Compilación**  
   Navega a la carpeta `programa/` y ejecuta:

   ```sh
   bcc32c main.c menu.c datos.c auxiliares.c config.c -o programa
   ```

   (Asegúrarse de incluir todos los archivos .c necesarios.)

2. **Ejecución**
   ```sh
   ./programa.exe
   ```

3. Los archivos de datos `.txt` deben estar presentes en la misma carpeta donde se ejecuta el programa.

## Ejemplo de Uso

- Al iniciar el programa, se muestra un menú principal con opciones para registrar libros, clientes, realizar pedidos y consultar información.
- El sistema valida entradas como cédulas, teléfonos y fechas para asegurar integridad de los datos.
