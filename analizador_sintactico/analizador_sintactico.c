#include "analizador_sintactico.h"
#include "../analizador_lexico/analizador_lexico.h"
#include <stdio.h>
#include <stdlib.h>

ComponenteLexico *componente_lexico;

void imprimir_componentes_lexicos() {
    siguiente_componente_lexico(componente_lexico);
    while (componente_lexico->componente_lexico != EOF) {
        printf("%s %d\n", componente_lexico->lexema, componente_lexico->componente_lexico);
        free(componente_lexico->lexema);
        siguiente_componente_lexico(componente_lexico);
    }
    printf("%s\n", componente_lexico->lexema);
    free(componente_lexico);
}

void iniciar_analizador_sintactico() {
    componente_lexico = (ComponenteLexico *) malloc(sizeof (ComponenteLexico ));
    imprimir_componentes_lexicos();
}


