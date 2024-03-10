#include "analizador_sintactico.h"
#include "../analizador_lexico/analizador_lexico.h"
#include <stdio.h>


void imprimir_componentes_lexicos() {
    ComponenteLexico componente_lexico;
    do {
        componente_lexico = siguiente_componente_lexico();
        printf("%s %d\n", componente_lexico.lexema, componente_lexico.componente_lexico);
    } while (componente_lexico.componente_lexico != EOF);
}

void iniciar_analizador_sintactico(char *nombre_archivo) {
    iniciar_analizador_lexico(nombre_archivo);
    imprimir_componentes_lexicos();
}


