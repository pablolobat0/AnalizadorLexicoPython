#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tabla_de_simbolos/tabla_de_simbolos.h"
#include "analizador_sintactico/analizador_sintactico.h"
#include "gestion_de_errores/gestion_de_errores.h"
#include "lex.yy.h"

bool es_fichero_python(char *nombre_fichero);

int main(int argc, char **argv) {
    if (argc != 2) {
        lanzar_error(ERR_NUMERO_DE_ARGUMENTOS_INCORRECTO, -1, -1);
        exit(EXIT_FAILURE);
    } else if (!es_fichero_python(argv[1])) {
        lanzar_error(ERR_FICHERO_NO_PYTHON, -1, -1);
        exit(EXIT_FAILURE);
    }

    abrir_archivo_fuente(argv[1]);
    crear_tabla_de_simbolos();
    printf("Contenido de la tabla de símbolos después de su inicializacióon:\n");
    imprimir_tabla_de_simbolos();
    printf("\nIdentificación de los componentes léxicos:\n");
    iniciar_analizador_sintactico();
    printf("\nContenido de la tabla de símbolos después del análisis léxico:\n");
    imprimir_tabla_de_simbolos();
    destruir_tabla_de_simbolos();
}

bool es_fichero_python(char *nombre_fichero) {
    char *last_dot = strrchr(nombre_fichero, '.');
    if (last_dot == NULL || strcmp(last_dot, ".py") != 0) {
        return false;
    }
    return true;
}
