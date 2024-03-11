#include "analizador_sintactico.h"
#include "../analizador_lexico/analizador_lexico.h"
#include <stdio.h>
#include <stdlib.h>

void liberar_memoria_componentes_lexicos(ComponenteLexico *componente_lexico);

void imprimir_componentes_lexicos() {
    ComponenteLexico *componente_lexico = siguiente_componente_lexico();
    while (componente_lexico->componente_lexico != EOF) {
        printf("%s %d\n", componente_lexico->lexema, componente_lexico->componente_lexico);
        liberar_memoria_componentes_lexicos(componente_lexico);
        componente_lexico = siguiente_componente_lexico();
    }
}

// Libera la memoria de los componentes lexicos que no se introducen en la tabla de simbolos
void liberar_memoria_componentes_lexicos(ComponenteLexico *componente_lexico) {
    if (componente_lexico->componente_lexico >= YIELD) {
        free(componente_lexico->lexema);
        free(componente_lexico);
    }
}

void iniciar_analizador_sintactico() {
    imprimir_componentes_lexicos();
}


