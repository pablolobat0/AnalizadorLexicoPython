#include "analizador_sintactico.h"
#include "analizador_lexico.h"
#include <stdio.h>
#include <string.h>


void imprimir_componentes_lexicos() {
    ComponenteLexico componente_lexico;
    do {
        componente_lexico = siguiente_componente_lexico();
        printf("Componente lexico: %d Lexema: %s\n", componente_lexico.componente_lexico, componente_lexico.lexema);
    } while (componente_lexico.componente_lexico != EOF);
}

void iniciar_analizador_sintactico(char *nombre_archivo) {
    iniciar_analizador_lexico(nombre_archivo);
    imprimir_componentes_lexicos();
}


