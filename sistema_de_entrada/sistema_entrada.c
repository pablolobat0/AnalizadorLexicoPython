#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sistema_entrada.h"

#define TAM_BUFFER 64
#define TAM_BLOQUE ((TAM_BUFFER / 2) -1)

typedef struct {
    char buffer[TAM_BUFFER];
    int inicio, delantero; // Punteros al inicio y al final del lexema
    bool ha_retrocedido_de_otro_bloque; // Evita sobreescribir un bloque por el retroceso de delantero
} Buffer;

Buffer buffer;
FILE *archivo_fuente;

void abrir_archivo_fuente(char *nombre_archivo);
void inicializar_buffer();
size_t cargar_bloque(char *bloque);
int comprobar_posicion_delantero();
int cargar_bloque_y_mover_delantero(int nueva_posicion_delantero, int posicion_a_devolver, int desplazamiento_buffer);
bool comprobar_retroceso_otro_bloque(int nueva_posicion_delantero);
void comprobar_elementos_leidos(size_t elementos_leidos, int desplazamiento_buffer);
int calcular_longitud_lexema();
void crear_lexema(char *lexema, int longitud);


void iniciar_sistema_entrada(char *nombre_archivo) {
    abrir_archivo_fuente(nombre_archivo);
    inicializar_buffer();
}

void abrir_archivo_fuente(char *nombre_archivo) {
    archivo_fuente = fopen(nombre_archivo, "r");
    if (archivo_fuente == NULL) {
        perror("Error al abrir el archivo.\n");
        exit(EXIT_FAILURE);
    }
}

void inicializar_buffer() {
    size_t elementos_leidos = cargar_bloque(buffer.buffer);
    if (elementos_leidos < TAM_BLOQUE) {
        buffer.buffer[elementos_leidos] = EOF;
    }

    buffer.buffer[TAM_BLOQUE] = EOF;
    buffer.buffer[TAM_BUFFER - 1] = EOF;
    buffer.inicio = 0;
    buffer.delantero = 0;
    buffer.ha_retrocedido_de_otro_bloque = false;
}

size_t cargar_bloque(char *bloque) {
    return fread(bloque, sizeof(char), TAM_BLOQUE, archivo_fuente);
}

int siguiente_caracter() {
    buffer.delantero++;
    if (buffer.buffer[buffer.delantero] == EOF) {
        return comprobar_posicion_delantero();
    }

    return buffer.buffer[buffer.delantero - 1];
}

int comprobar_posicion_delantero() {
    switch (buffer.delantero) {
        case TAM_BLOQUE: // Final bloque A
            return cargar_bloque_y_mover_delantero(TAM_BLOQUE + 1, TAM_BLOQUE - 1, TAM_BUFFER / 2);
        case TAM_BUFFER - 1: // Final bloque B
            return cargar_bloque_y_mover_delantero(0, TAM_BUFFER - 2, 0);
        default:
            return buffer.buffer[buffer.delantero - 1];
    }
}

int cargar_bloque_y_mover_delantero(int nueva_posicion_delantero, int posicion_a_devolver, int desplazamiento_buffer) {
    size_t elementos_leidos;
    if (comprobar_retroceso_otro_bloque(nueva_posicion_delantero))
        return buffer.buffer[posicion_a_devolver];

    elementos_leidos = cargar_bloque(buffer.buffer + (desplazamiento_buffer));
    comprobar_elementos_leidos(elementos_leidos, desplazamiento_buffer);

    buffer.delantero = nueva_posicion_delantero;
    return buffer.buffer[posicion_a_devolver];
}

bool comprobar_retroceso_otro_bloque(int nueva_posicion_delantero) {
    if (buffer.ha_retrocedido_de_otro_bloque) {
        buffer.ha_retrocedido_de_otro_bloque = false;
        buffer.delantero = nueva_posicion_delantero;
        return true;
    }
    return false;
}

void comprobar_elementos_leidos(size_t elementos_leidos, int desplazamiento_buffer) {
    if (elementos_leidos < TAM_BLOQUE) {
        buffer.buffer[elementos_leidos + desplazamiento_buffer] = EOF;
    }
}

char *obtener_lexema() {
    // Reservamos memoria para el lexema y el \0 final
    int longitud = calcular_longitud_lexema();

    char *lexema = (char *) malloc(sizeof(char) * (longitud + 1));
    if (lexema == NULL) {
        printf("Error al crear el lexema\n");
        exit(EXIT_FAILURE);
    }

    crear_lexema(lexema, longitud);
    return lexema;
}

int calcular_longitud_lexema() {
    if (buffer.inicio > buffer.delantero) {
        return TAM_BUFFER - 1 - buffer.inicio + buffer.delantero;
    } else {
        if (buffer.delantero > TAM_BLOQUE && buffer.inicio < TAM_BLOQUE) {
            return buffer.delantero - buffer.inicio - 1;
        } else {
            return buffer.delantero - buffer.inicio;
        }
    }
}

void crear_lexema(char *lexema, int longitud) {
    for (int i = 0; i < longitud; i++) {
        lexema[i] = (char) buffer.buffer[buffer.inicio];
        buffer.inicio++;
        if (buffer.inicio == (TAM_BUFFER - 1)) { // Saltamos el final de bloque
            buffer.inicio = 0;
        } else if (buffer.inicio == TAM_BLOQUE) {
            buffer.inicio++;
        }
    }
    lexema[longitud] = '\0';
}

void retroceder_caracter() {
    buffer.delantero--;
    if (buffer.buffer[buffer.delantero] == EOF) {
        buffer.delantero--;
        buffer.ha_retrocedido_de_otro_bloque = true;
    } else if (buffer.delantero < 0) {
        buffer.ha_retrocedido_de_otro_bloque = true;
        buffer.delantero = TAM_BUFFER - 2;
    }
}

void saltar_caracter() {
    buffer.inicio = buffer.delantero;
}

void cerrar_sistema_de_entrada() {
    fclose(archivo_fuente);
}