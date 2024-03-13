//
// Created by pablo on 8/03/24.
//

#include "gestion_de_errores.h"
#include <stdio.h>

void lanzar_error(int codigo_de_error, int fila, int columna) {
    switch (codigo_de_error) {
        case ERR_NUMERO_DE_ARGUMENTOS_INCORRECTO:
            printf("ERROR: Número incorrecto de argumentos. Uso: ./ejecutable archivo.py\n");
            break;
        case ERR_FICHERO_NO_ABRE:
            printf("ERROR: No se pudo abrir el archivo de código fuente.\n");
            break;
        case ERR_FICHERO_NO_PYTHON:
            printf("ERROR: El archivo de código fuente no es un archivo de Python válido. Uso: ./ejecutable archivo.py\n");
            break;
            case ERR_CARACTER_NO_RECONOCIDO:
            printf("Error en la fila %d y en la columna %d: Carácter no reconocido.\n", fila, columna);
            break;
    }
}