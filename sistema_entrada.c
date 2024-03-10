#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sistema_entrada.h"

#define TAM_BUFFER 128
#define TAM_BLOQUE ((TAM_BUFFER / 2) -1)

char buffer[TAM_BUFFER];
int inicio, delantero;

bool ha_retrocedido_de_otro_bloque = false;

FILE *archivo_fuente;

size_t cargar_bloque(char *bloque) {
    return fread(bloque, sizeof(char), TAM_BLOQUE, archivo_fuente);
}

void iniciar_sistema_entrada(char *nombre_archivo) {
    archivo_fuente = fopen(nombre_archivo, "r");
    if (archivo_fuente == NULL) {
        perror("Error al abrir el archivo.\n");
        exit(EXIT_FAILURE);
    }
    // Inicializacion del doble buffer
    inicio = 0;
    delantero = 0;
    buffer[TAM_BLOQUE] = EOF;
    buffer[TAM_BUFFER - 1] = EOF;
    size_t elementos_leidos = cargar_bloque(buffer);
    if (elementos_leidos < TAM_BLOQUE) {
        buffer[elementos_leidos] = EOF;
    }
}

int siguiente_caracter() {
    delantero++;
    size_t elementos_leidos;
    if (buffer[delantero] == EOF) {
        switch (delantero) {
            case TAM_BLOQUE: // Final bloque A
                if (!ha_retrocedido_de_otro_bloque) {
                    elementos_leidos = cargar_bloque(buffer + (TAM_BUFFER / 2));
                    if (elementos_leidos < (TAM_BUFFER / 2) - 1) {
                        buffer[elementos_leidos + (TAM_BUFFER / 2)] = EOF;
                    }
                }
                ha_retrocedido_de_otro_bloque = false;
                delantero++;
                return buffer[delantero - 2];
            case TAM_BUFFER - 1: // Final bloque B
                if (!ha_retrocedido_de_otro_bloque) {
                    elementos_leidos = cargar_bloque(buffer);
                    if (elementos_leidos < (TAM_BUFFER / 2) - 1) {
                        buffer[elementos_leidos] = EOF;
                    }
                }
                ha_retrocedido_de_otro_bloque = false;
                delantero = 0;
                return buffer[TAM_BUFFER - 2];
        }
    }

    return buffer[delantero - 1];
}

char *get_lexema() {
    // Reservamos memoria para el lexema y el \0 final
    int longitud = 0;
    if (inicio > delantero) {
        longitud = TAM_BUFFER - 1 - inicio + delantero;
    } else {
        if (delantero > ((TAM_BUFFER / 2) - 1) && inicio < ((TAM_BUFFER / 2) - 1)) {
            longitud = delantero - inicio - 1;
        } else {
            longitud = delantero - inicio;
        }
    }
    char *lexema = (char *) malloc(sizeof(char) * (longitud + 1));
    if (lexema == NULL) {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < longitud; i++) {
        lexema[i] = (char) buffer[inicio];
        inicio++;
        if (inicio == (TAM_BUFFER - 1)) { // Saltamos el final de bloque
            inicio = 0;
        } else if (inicio == (TAM_BUFFER / 2) - 1) {
            inicio++;
        }
    }
    lexema[longitud] = '\0';
    return lexema;
}

void retroceder_caracter() {
    delantero--;
    if (buffer[delantero] == EOF) {
        delantero--;
        ha_retrocedido_de_otro_bloque = true;
    } else if (delantero < 0) {
        ha_retrocedido_de_otro_bloque = true;
        delantero = TAM_BUFFER - 2;
    }
}

void saltar_caracter() {
    inicio = delantero;
}

void cerrar_sistema_de_entrada() {
    fclose(archivo_fuente);
}