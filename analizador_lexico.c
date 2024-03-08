#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "analizador_lexico.h"
#include "sistema_entrada.h"
#include "gestion_de_errores.h"

int fila = 1, columna = 1;

void iniciar_analizador_lexico(char *nombre_archivo) {
    iniciar_sistema_entrada(nombre_archivo);
}

void automata_alfanumerico() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (isalnum(caracter_actual) || caracter_actual == '_'); // Caracter alfanumerico o _
}

int automata_numero() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (isdigit(caracter_actual) || caracter_actual == '_');
    return caracter_actual;
}

void automata_exponente(int caracter_actual) {
    if (caracter_actual != 'e')
        return;
    caracter_actual = siguiente_caracter();
    if (caracter_actual != '-' || !isdigit(caracter_actual))
        return;
    columna++;
    automata_numero();
}

void automata_punto_flotante() {
    int ultimo_caracter = automata_numero();
    automata_exponente(ultimo_caracter);
}

void automata_binario() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (caracter_actual == '1' || caracter_actual == '0' || caracter_actual == '_');
}

void automata_octal() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while ((caracter_actual >= 48 && caracter_actual <= 55) || caracter_actual == '_'); // Numero del 0 al 7 o _
}

bool es_hexadecimal(int caracter) {
    return ((caracter >= 48 && caracter <= 57) || (caracter >= 65 && caracter <= 70) ||
            (caracter >= 97 && caracter <= 102) || caracter == '_');
}

void automata_hexadecimal() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (es_hexadecimal(
            caracter_actual)); // Numero del 0 al 9, letra de la 'A' a la 'F', letra de la 'a' a la 'f' o _
}

void automata_entero() {
    int ultimo_caracter = automata_numero();

    if (ultimo_caracter == '.')
        automata_punto_flotante();
}

void automata_numerico() {
    int caracter_actual = siguiente_caracter();
    columna++;
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
    columna = 0;
    fila++;
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

// Entiendase triple los que pueden ser de la forma '*', '*=', '**', o '**='
ComponenteLexico automata_delimitadores_triples(int caracter_actual, int anterior_caracter) {
    if (caracter_actual == '=') { // Forma '*'
        return aceptar_lexema();
    } else if (anterior_caracter == caracter_actual) {
        if (siguiente_caracter() == '=') { // Forma '**='
            return aceptar_lexema();
        } else { // Forma '**'
            retroceder_caracter();
            return aceptar_lexema();
        }
    } else { // Forma '*'
        retroceder_caracter();
        return aceptar_lexema_simbolo();
    }
}

// Entiendase doble los que pueden ser de la forma '&' o '&='
ComponenteLexico automata_operadores_delimitadores_dobles(int caracter_actual) {
    if (caracter_actual == '=') { // Forma '&='
        return aceptar_lexema();
    } else { // Forma '&'
        retroceder_caracter();
        return aceptar_lexema_simbolo();
    }
}

bool es_posible_operador_delimitador_doble(int caracter) {
    return (caracter == ':' || caracter == '=' || caracter == '!' || caracter == '+' || caracter == '%' ||
            caracter == '@' || caracter == '&' || caracter == '|' || caracter == '^');
}

bool es_posible_delimitador_triple(int caracter) {
    return (caracter == '/' || caracter == '*' || caracter == '<' || caracter == '>');
}

// Entiendase simple como los que tienen la forma '{' solo
bool es_simbolo_simple(int caracter) {
    return (caracter == ';' || caracter == ',' || caracter == '[' || caracter == ']' || caracter == '(' ||
            caracter == ')' || caracter == '{' || caracter == '}');
}

ComponenteLexico automata_simbolos(int caracter_anterior) {
    int caracter_actual = siguiente_caracter();

    if (es_posible_delimitador_triple(caracter_anterior)) {
        return automata_delimitadores_triples(caracter_actual, caracter_anterior);
    } else if (es_posible_operador_delimitador_doble(caracter_anterior)) {
        return automata_operadores_delimitadores_dobles(caracter_actual);
    } else if (caracter_anterior == '.') {
        if (isdigit(caracter_actual)) {
            automata_punto_flotante();
            retroceder_caracter(); // Se retrocede un caracter para que no quede sin analizar
            return aceptar_lexema();
        } else {
            retroceder_caracter();
            return aceptar_lexema_simbolo();
        }
    } else if (caracter_anterior == '-') { // Es un caso a parte porque puede tener las formas '-=' o '->'
        if (caracter_actual == '=' || caracter_actual == '>') {
            return aceptar_lexema();
        } else {
            retroceder_caracter();
            return aceptar_lexema_simbolo();
        }
    }
}

ComponenteLexico automata_string(int caracter_inicial) {
    int caracter_actual = caracter_inicial;
    int caracter_anterior;

    do {
        caracter_anterior = caracter_actual; // Guardamos el anterior para comprobar '\'
        caracter_actual = siguiente_caracter();
        columna++;
        if (caracter_actual == '\n') {
            fila++;
            columna = 1;
        }
    } while (caracter_actual != caracter_inicial || caracter_anterior == '\\');
    return aceptar_lexema();
}

ComponenteLexico siguiente_componente_lexico() {
    int caracter_actual;
    ComponenteLexico componente_lexico;

    int componente_lexico_encontrado = 0;
    while (!componente_lexico_encontrado) {
        caracter_actual = siguiente_caracter();
        columna++;
        if (isalpha(caracter_actual) || caracter_actual == '_') {
            automata_alfanumerico();
            componente_lexico_encontrado = 1;
        } else if (isdigit(caracter_actual)) {
            automata_numerico();
            componente_lexico_encontrado = 1;
        } else if (caracter_actual == 32 || caracter_actual == '\t') {
            saltar_caracter();
        } else if (caracter_actual == '\n') {
            fila++;
            columna = 0;
            saltar_caracter();
        } else if (caracter_actual == '\'' || caracter_actual == '\"') {
            return automata_string(caracter_actual);
        } else if (es_simbolo_simple(caracter_actual)) {
            return aceptar_lexema_simbolo(); // Cuando es un simbolo se devuelve el ASCII
        } else if (es_posible_operador_delimitador_doble(caracter_actual) ||
                   es_posible_delimitador_triple(caracter_actual) || caracter_actual == '.' || caracter_actual == '-') {
            return automata_simbolos(caracter_actual);
        } else if (caracter_actual == '#') {
            saltar_linea();
        } else if (caracter_actual == EOF) {
            componente_lexico.lexema = NULL;
            componente_lexico.componente_lexico = EOF;
            cerrar_sistema_de_entrada();
            return componente_lexico;
        } else {
            gestionar_error("Error léxico. Cáracter no reconocido", fila, columna);
            saltar_caracter();
        }

    }

    retroceder_caracter(); // Se retrocede un caracter para que no quede sin analizar

    return aceptar_lexema();
}

