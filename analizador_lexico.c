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
    } while (isalnum(caracter_actual) || caracter_actual == '_'); // Caracter alfanumerico o _
}

void automata_punto_flotante() {
    int caracter_actual, componente_lexico_encontrado = 0;
    int estado = 0;
    while (!componente_lexico_encontrado) {
        switch (estado) {
            case 0:
                do {
                    caracter_actual = siguiente_caracter();
                } while (isdigit(caracter_actual) || caracter_actual == '_');

                if (caracter_actual == 'e')
                    estado = 1;
                else
                    componente_lexico_encontrado = 1;
                break;
            case 1:
                caracter_actual = siguiente_caracter();
                if (caracter_actual == '-' || isdigit(caracter_actual))
                    estado = 2;
                break;
            case 2:
                do {
                    caracter_actual = siguiente_caracter();
                } while (isdigit(caracter_actual) || caracter_actual == '_');
                componente_lexico_encontrado = 1;
        }
    }
}

void automata_binario() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
    } while (caracter_actual == '1' || caracter_actual == '0' || caracter_actual == '_');
}

void automata_octal() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
    } while ((caracter_actual >= 48 && caracter_actual <= 55) || caracter_actual == '_'); // Numero del 0 al 7 o _
}

void automata_hexadecimal() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
    } while ((caracter_actual >= 48 && caracter_actual <= 57) || (caracter_actual >= 65 && caracter_actual <= 70) ||
             (caracter_actual >= 97 && caracter_actual <= 102) ||
             caracter_actual == '_'); // Numero del 0 al 9, letra de la 'A' a la 'F', letra de la 'a' a la 'f' o _
}

void automata_entero() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
    } while (isdigit(caracter_actual) || caracter_actual == '_');

    if (caracter_actual == '.') {
        automata_punto_flotante();
    }
}

void automata_numerico() {
    int caracter_actual = siguiente_caracter();
    switch (caracter_actual) { // Clasificamos los numero segun su 2 caracter
        case '.':
        case 'e':
            automata_punto_flotante();
            break;
        case 'b':
        case 'B':
            automata_binario();
            break;
        case 'o':
        case 'O':
            automata_octal();
            break;
        case 'x':
        case 'X':
            automata_hexadecimal();
            break;
        default:
            if (isdigit(caracter_actual))
                automata_entero();
    }
}

void saltar_linea() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
    } while (caracter_actual != '\n');
    saltar_caracter(); // Borramos el buffer
}

ComponenteLexico aceptar_lexema() {
    ComponenteLexico componente_lexico;
    componente_lexico.lexema = get_lexema();
    componente_lexico.componente_lexico = buscar_e_insertar_en_tabla_de_simbolos(componente_lexico.lexema);

    return componente_lexico;
}

ComponenteLexico aceptar_lexema_simbolo() {
    ComponenteLexico componente_lexico;
    componente_lexico.lexema = get_lexema();
    componente_lexico.componente_lexico = (int) componente_lexico.lexema[0]; // Valor ASCII

    return componente_lexico;
}

// Entiendase doble los que pueden ser de la forma '*', '*=', '**', o '**='
ComponenteLexico automata_operadores_delimitadores_dobles(int caracter_actual, int anterior_caracter) {
    if (caracter_actual == '=') {
        return aceptar_lexema();
    } else if (anterior_caracter == caracter_actual) {
        if (siguiente_caracter() == '=') {
            return aceptar_lexema();
        } else {
            retroceder_caracter();
            return aceptar_lexema();
        }
    } else {
        retroceder_caracter();
        return aceptar_lexema_simbolo();
    }
}

// Entiendase simple los que pueden ser de la forma '&' o '&='
ComponenteLexico automata_operadores_delimitadores_simples(int caracter_actual) {
    if (caracter_actual == '=') {
        return aceptar_lexema();
    } else {
        retroceder_caracter();
        return aceptar_lexema_simbolo();
    }
}

ComponenteLexico automata_simbolos(int estado) {
    int caracter_actual = siguiente_caracter();
    switch (estado) {
        case '.':
            if (isdigit(caracter_actual)) {
                automata_punto_flotante();
                retroceder_caracter(); // Se retrocede un caracter para que no quede sin analizar
                return aceptar_lexema();
            } else {
                retroceder_caracter();
                return aceptar_lexema_simbolo();
            }
            break;
        case '/':
        case '*':
        case '<':
        case '>':
            return automata_operadores_delimitadores_dobles(caracter_actual, estado);
        case ':':
        case '=':
        case '!':
        case '+':
        case '%':
        case '@':
        case '&':
        case '|':
        case '^':
            return automata_operadores_delimitadores_simples(caracter_actual);
        case '-':
            if (caracter_actual == '=' || caracter_actual == '>') {
                return aceptar_lexema();
            } else {
                retroceder_caracter();
                return aceptar_lexema_simbolo();
            }
    }
}

ComponenteLexico siguiente_componente_lexico() {
    int caracter_actual;
    ComponenteLexico componente_lexico;
    int componente_lexico_encontrado = 0;
    while (!componente_lexico_encontrado) {
        caracter_actual = siguiente_caracter();
        if (isalpha(caracter_actual) || caracter_actual == '_') {
            automata_alfanumerico();
            componente_lexico_encontrado = 1;
        } else if (isdigit(caracter_actual)) {
            automata_numerico();
            componente_lexico_encontrado = 1;
        } else {
            switch (caracter_actual) {
                case EOF:
                    componente_lexico.lexema = NULL;
                    componente_lexico.componente_lexico = EOF;
                    cerrar_sistema_de_entrada();
                    return componente_lexico;
                case '#': // Comentarios de una linea
                    saltar_linea();
                    break;
                case ';':
                case ',':
                case '[':
                case ']':
                case '(':
                case ')':
                case '{':
                case '}':
                    return aceptar_lexema_simbolo(); // Cuando es un simbolo se devuelve el ASCII
                case '.':
                case '/':
                case '*':
                case '<':
                case '>':
                case ':':
                case '=':
                case '!':
                case '-':
                case '+':
                case '%':
                case '@':
                case '&':
                case '|':
                case '^':
                    return automata_simbolos(caracter_actual);
                default: // Si no es ninguna opcion, el caracter no tiene porque analizarse
                    saltar_caracter();
            }
        }
    }
    retroceder_caracter(); // Se retrocede un caracter para que no quede sin analizar
    return aceptar_lexema();
}

