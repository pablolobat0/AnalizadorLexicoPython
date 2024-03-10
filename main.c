#include <stdio.h>
#include <stdlib.h>
#include "tabla_de_simbolos/tabla_de_simbolos.h"
#include "analizador_sintactico/analizador_sintactico.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("ERROR! Número de argumentos incorrecto. Ejemplo: ./ejecutable archivo.py\n");
        exit(EXIT_FAILURE);
    }
    crear_tabla_de_simbolos();
    iniciar_analizador_sintactico(argv[1]);
    destruir_tabla_de_simbolos();
}
