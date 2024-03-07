#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sistema_entrada.h"

#define TAM_BUFFER 256

char buffer[TAM_BUFFER];
int inicio, delantero;


FILE *archivo_fuente;


size_t cargar_bloque(char *bloque) {
    return fread(bloque, sizeof(char), (TAM_BUFFER / 2) - 1, archivo_fuente);
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
    buffer[(TAM_BUFFER / 2) - 1] = EOF;
    buffer[TAM_BUFFER - 1] = EOF;
    cargar_bloque(buffer);
}

int siguiente_caracter() {
    delantero++;
    size_t elementos_leidos;
    if (buffer[delantero] == EOF) {
        switch (delantero) {
            case (TAM_BUFFER / 2) - 1: // Final bloque A
                elementos_leidos = cargar_bloque(buffer + (TAM_BUFFER / 2));
                if (elementos_leidos < (TAM_BUFFER / 2) - 1) {
                    buffer[elementos_leidos + (TAM_BUFFER / 2)] = EOF;
                }
                delantero++;
                return buffer[delantero - 2];
            case TAM_BUFFER - 1: // Final bloque B
                elementos_leidos = cargar_bloque(buffer);
                if (elementos_leidos < (TAM_BUFFER / 2) - 1) {
                    buffer[elementos_leidos] = EOF;
                }
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
        longitud = TAM_BUFFER - 2 - inicio + delantero;
    } else {
        longitud = delantero - inicio;
    }
    char *lexema = (char *) malloc(sizeof(char) * (longitud + 1));
    if (lexema == NULL) {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < longitud; i++) {
        lexema[i] = (char) buffer[inicio];
        inicio++;
        if (buffer[inicio] == EOF) { // Saltamos el final de bloque
            inicio++;
        }
    }
    lexema[longitud] = '\0';
    return lexema;
}

void retroceder_caracter() {
    delantero -= 1;
}

void saltar_caracter() {
    inicio = delantero;
}

void cerrar_sistema_de_entrada() {
    fclose(archivo_fuente);
}