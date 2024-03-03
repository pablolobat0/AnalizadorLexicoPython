#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sistema_entrada.h"

#define TAM 64

typedef struct {
    char lexema[TAM];
    int longitud;
} Buffer;

Buffer buffer = {"", 0};

FILE* archivo_fuente;


void iniciar_sistema_entrada(char* nombre_archivo) {
    archivo_fuente = fopen(nombre_archivo, "r");
    if (archivo_fuente == NULL) {
        perror("Error al abrir el archivo.\n");
        exit(EXIT_FAILURE);
    }
}

int siguiente_caracter() {
    int caracter = fgetc(archivo_fuente);
    buffer.lexema[buffer.longitud] = (char) caracter;
    buffer.longitud += 1;
    return caracter;
}

void borrar_buffer() {
    memset(buffer.lexema, 0, sizeof(buffer.lexema));
    buffer.longitud = 0;
}

char *get_lexema() {
    // Reservamos memoria para el lexema y el \0 final
    char *lexema = (char*) malloc(sizeof(char ) * (buffer.longitud + 1));
    for (int i = 0; i < buffer.longitud; i++) {
        lexema[i] = buffer.lexema[i];
    }
    lexema[buffer.longitud] = '\0';
    borrar_buffer();
    return lexema;
}

void retroceder_caracter() {
    buffer.longitud -= 1;
    fseek(archivo_fuente, -1, SEEK_CUR);
}

void saltar_caracter() {
    borrar_buffer();
}

void cerrar_sistema_de_entrada() {
    fclose(archivo_fuente);
}