//
// Created by pablo on 8/03/24.
//

#include "gestion_de_errores.h"
#include <stdio.h>

void gestionar_error(char* mensaje_error, int fila, int columna) {
    printf("%s Fila: %d Columna: %d\n", mensaje_error, fila, columna);
}