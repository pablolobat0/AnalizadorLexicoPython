#include "analizador_sintactico.h"
#include "../analizador_lexico/analizador_lexico.h"
#include <stdio.h>
#include <stdlib.h>

void imprimir_componentes_lexicos() {
    ComponenteLexico *componente_lexico = siguiente_componente_lexico();
    while (componente_lexico->componente_lexico != EOF) {
        printf("%s %d\n", componente_lexico->lexema, componente_lexico->componente_lexico);
        free(componente_lexico->lexema);
        componente_lexico = siguiente_componente_lexico();
    }
    printf("%s\n", componente_lexico->lexema);
    free(componente_lexico);
}

void iniciar_analizador_sintactico() {
    iniciar_analizador_lexico();
    imprimir_componentes_lexicos();
}


