#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "analizador_lexico.h"
#include "sistema_entrada.h"


void iniciar_analizador_lexico(char *nombre_archivo) {
    iniciar_sistema_entrada(nombre_archivo);
}

void automata_alfanumerico() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
    } while (isalnum(caracter_actual) || caracter_actual == '_');
    retroceder_caracter();
}

ComponenteLexico aceptar_lexema() {
    ComponenteLexico componente_lexico;
    componente_lexico.lexema = get_lexema();
    if (strcmp(componente_lexico.lexema, ".") == 0) {
        componente_lexico.componente_lexico = '.';
        return componente_lexico;
    }

    componente_lexico.componente_lexico = buscar_e_insertar_en_tabla_de_simbolos(componente_lexico.lexema);

    return componente_lexico;
}

ComponenteLexico aceptar_lexema_simbolo() {
    ComponenteLexico componente_lexico;
    componente_lexico.lexema = get_lexema();
    componente_lexico.componente_lexico = (int) componente_lexico.lexema[0];
    return componente_lexico;
}

ComponenteLexico siguiente_componente_lexico() {
    int caracter_actual;
    ComponenteLexico componente_lexico;
    int estado = 0, componente_lexico_encontrado = 0;
    while (!componente_lexico_encontrado) {
        caracter_actual = siguiente_caracter();
        switch (estado) {
            case 0:
                if (isalpha(caracter_actual) || caracter_actual == '_') {
                    automata_alfanumerico();
                    componente_lexico_encontrado = 1;
                } else {
                    switch (caracter_actual) {
                        case EOF:
                            componente_lexico.lexema = NULL;
                            componente_lexico.componente_lexico = EOF;
                            return componente_lexico;
                        case '.':
                        case ',':
                        case '[':
                        case ']':
                        case '(':
                        case ')':
                        case '{':
                        case '}':
                            return aceptar_lexema_simbolo();
                        default:
                            saltar_caracter();
                    }
                }
                break;
        }
    }

    return aceptar_lexema();
}

