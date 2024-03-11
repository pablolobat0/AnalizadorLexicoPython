#include <stdio.h>
#include "stdlib.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "analizador_lexico.h"
#include "../sistema_de_entrada/sistema_entrada.h"
#include "../gestion_de_errores/gestion_de_errores.h"

int fila = 1, columna = 1;

bool es_hexadecimal(int caracter);
bool es_posible_operador_delimitador_doble(int caracter);
bool es_posible_delimitador_triple(int caracter);
bool es_simbolo_simple(int caracter);
bool tiene_comillas_triples(int caracter_inicial);
int comprobar_operadores_y_delimitadores(char *lexema);
ComponenteLexico *automata_alfanumerico();
int automata_numero();
ComponenteLexico *automata_exponente();
ComponenteLexico *automata_punto_flotante();
ComponenteLexico *automata_binario();
ComponenteLexico *automata_octal();
ComponenteLexico *automata_hexadecimal();
ComponenteLexico *automata_entero();
ComponenteLexico *automata_numerico();
ComponenteLexico *automata_delimitadores_triples(int caracter_actual, int caracter_anterior);
ComponenteLexico *automata_operadores_delimitadores_dobles(int caracter_actual, int caracter_anterior);
ComponenteLexico *automata_simbolos(int caracter_anterior);
ComponenteLexico *automata_string_triple_comilla();
ComponenteLexico *automata_string(int caracter_inicial);
ComponenteLexico *aceptar_lexema_y_buscar_e_insertar_en_tabla_de_simbolos();
ComponenteLexico *aceptar_lexema(int comp);
ComponenteLexico *aceptar_lexema_operadores_y_delimitadores();
ComponenteLexico *retroceder_y_aceptar_lexema(int comp);
void saltar_linea();
ComponenteLexico *terminar_analisis_lexico();

ComponenteLexico *siguiente_componente_lexico() {
    while (true) {
        int caracter_actual = siguiente_caracter();
        columna++;
        if (isalpha(caracter_actual) || caracter_actual == '_') {
            return automata_alfanumerico();
        } else if (isdigit(caracter_actual)) {
            return automata_numerico();
        } else if (caracter_actual == 32 || caracter_actual == '\t') {
            saltar_caracter();
        } else if (caracter_actual == '\n') {
            fila++;
            columna = 0;
            saltar_caracter();
        } else if (es_simbolo_simple(caracter_actual)) {
            return aceptar_lexema(caracter_actual); // Cuando es un simbolo se devuelve el ASCII
        } else if (es_posible_operador_delimitador_doble(caracter_actual) ||
                   es_posible_delimitador_triple(caracter_actual) || caracter_actual == '.' || caracter_actual == '-') {
            return automata_simbolos(caracter_actual);
        } else if (caracter_actual == '\'' || caracter_actual == '\"') {
            return automata_string(caracter_actual);
        } else if (caracter_actual == '#') {
            saltar_linea();
        } else if (caracter_actual == EOF) {
            return terminar_analisis_lexico();
        } else {
            gestionar_error("Error léxico. Cáracter no reconocido", fila, columna);
            saltar_caracter();
        }
    }
}

ComponenteLexico *automata_alfanumerico() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (isalnum(caracter_actual) || caracter_actual == '_'); // Caracter alfanumerico o _

    retroceder_caracter(); // Se retrocede un caracter para que no quede sin analizar
    return aceptar_lexema_y_buscar_e_insertar_en_tabla_de_simbolos();
}

ComponenteLexico *automata_numerico() {
    int caracter_actual = siguiente_caracter();
    columna++;
    switch (caracter_actual) { // Clasificamos los numero segun su 2 caracter
        case 'e':
        case 'E':
            return automata_exponente();
        case '.':
            return automata_punto_flotante();
        case 'b':
        case 'B':
            return automata_binario();
        case 'o':
        case 'O':
            return automata_octal();
        case 'x':
        case 'X':
            return automata_hexadecimal();
        default:
            if (isdigit(caracter_actual))
                return automata_entero();
    }

    return retroceder_y_aceptar_lexema(INT);
}

ComponenteLexico *automata_exponente() {
    int ultimo_caracter = automata_numero();
    if (ultimo_caracter == '-')
        ultimo_caracter = automata_numero();
    if (ultimo_caracter == 'j' || ultimo_caracter == 'J')
        return aceptar_lexema(IMAGINARY);

    return retroceder_y_aceptar_lexema(FLOAT);
}

int automata_numero() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (isdigit(caracter_actual) || caracter_actual == '_');
    return caracter_actual;
}

ComponenteLexico *automata_punto_flotante() {
    int ultimo_caracter = automata_numero();
    if (ultimo_caracter == 'e' || ultimo_caracter == 'E')
        return automata_exponente();
    else if (ultimo_caracter == 'j' || ultimo_caracter == 'J')
        return aceptar_lexema(IMAGINARY);

    return retroceder_y_aceptar_lexema(FLOAT);
}

ComponenteLexico *automata_binario() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (caracter_actual == '1' || caracter_actual == '0' || caracter_actual == '_');

    return retroceder_y_aceptar_lexema(INT);
}

ComponenteLexico *automata_octal() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while ((caracter_actual >= '0' && caracter_actual <= '7') || caracter_actual == '_');

    return retroceder_y_aceptar_lexema(INT);
}

ComponenteLexico *automata_hexadecimal() {
    int caracter_actual;
    do {
        caracter_actual = siguiente_caracter();
        columna++;
    } while (es_hexadecimal(caracter_actual));
    return retroceder_y_aceptar_lexema(INT);
}

bool es_hexadecimal(int caracter) {
    return ((caracter >= '0' && caracter <= '9') || (caracter >= 'A' && caracter <= 'F') ||
            (caracter >= 'a' && caracter <= 'f') || caracter == '_');
}

ComponenteLexico *automata_entero() {
    int ultimo_caracter = automata_numero();

    if (ultimo_caracter == '.')
        return automata_punto_flotante();
    else if (ultimo_caracter == 'j' || ultimo_caracter == 'J')
        return aceptar_lexema(IMAGINARY);

    return retroceder_y_aceptar_lexema(INT);
}

// Entiendase simple como los que tienen la forma '{' solo
bool es_simbolo_simple(int caracter) {
    return (caracter == ';' || caracter == ',' || caracter == '[' || caracter == ']' || caracter == '(' ||
            caracter == ')' || caracter == '{' || caracter == '}');
}

ComponenteLexico *automata_simbolos(int caracter_anterior) {
    int caracter_actual = siguiente_caracter();

    if (es_posible_delimitador_triple(caracter_anterior)) {
        return automata_delimitadores_triples(caracter_actual, caracter_anterior);
    } else if (es_posible_operador_delimitador_doble(caracter_anterior)) {
        return automata_operadores_delimitadores_dobles(caracter_actual, caracter_anterior);
    } else if (caracter_anterior == '.') {
        if (isdigit(caracter_actual)) {
            return automata_punto_flotante();
        } else {
            retroceder_caracter();
            return aceptar_lexema(caracter_anterior);
        }
    } else if (caracter_anterior == '-') { // Es un caso a parte porque puede tener las formas '-=' o '->'
        if (caracter_actual == '=' || caracter_actual == '>') {
            return aceptar_lexema_operadores_y_delimitadores();
        } else {
            retroceder_caracter();
            return aceptar_lexema(caracter_anterior);
        }
    }
}

// Entiendase triple los que pueden ser de la forma '*', '*=', '**', o '**='
bool es_posible_delimitador_triple(int caracter) {
    return (caracter == '/' || caracter == '*' || caracter == '<' || caracter == '>');
}

ComponenteLexico *automata_delimitadores_triples(int caracter_actual, int caracter_anterior) {
    if (caracter_actual == '=') { // Forma '*'
        return aceptar_lexema_operadores_y_delimitadores();
    } else if (caracter_anterior == caracter_actual) {
        if (siguiente_caracter() == '=') { // Forma '**='
            return aceptar_lexema_operadores_y_delimitadores();
        } else { // Forma '**'
            retroceder_caracter();
            return aceptar_lexema_operadores_y_delimitadores();
        }
    } else { // Forma '*'
        retroceder_caracter();
        return aceptar_lexema(caracter_anterior);
    }
}

// Entiendase doble los que pueden ser de la forma '&' o '&='
bool es_posible_operador_delimitador_doble(int caracter) {
    return (caracter == ':' || caracter == '=' || caracter == '!' || caracter == '+' || caracter == '%' ||
            caracter == '@' || caracter == '&' || caracter == '|' || caracter == '^');
}

ComponenteLexico *automata_operadores_delimitadores_dobles(int caracter_actual, int caracter_anterior) {
    if (caracter_actual == '=') { // Forma '&='
        return aceptar_lexema_operadores_y_delimitadores();
    } else { // Forma '&'
        retroceder_caracter();
        return aceptar_lexema(caracter_anterior);
    }
}

ComponenteLexico *automata_string(int caracter_inicial) {
    if (tiene_comillas_triples(caracter_inicial))
        return automata_string_triple_comilla();

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
    return aceptar_lexema(STRING);
}

bool tiene_comillas_triples(int caracter_inicial) {
    if (caracter_inicial != '\"')
        return false;

    int numero_de_comillas = 1;
    int caracter_actual;
    while (numero_de_comillas != 3) {
        caracter_actual = siguiente_caracter();
        if (caracter_actual != '\"')
            return false;
        numero_de_comillas++;
    }
    return true;
}

ComponenteLexico *automata_string_triple_comilla() {
    int numero_de_comillas = 3;
    int caracter_actual;

    while (numero_de_comillas != 0) {
        caracter_actual = siguiente_caracter();
        columna++;
        if (caracter_actual == '\"')
            numero_de_comillas--;
    }
    return aceptar_lexema(STRING);
}

ComponenteLexico *aceptar_lexema_y_buscar_e_insertar_en_tabla_de_simbolos() {
    ComponenteLexico *componente_lexico = (ComponenteLexico*) malloc(sizeof(ComponenteLexico));
    if (componente_lexico == NULL) {
        printf("Error al crear el componente lexico\n");
        exit(EXIT_FAILURE);
    }
    componente_lexico->lexema = obtener_lexema();
    componente_lexico->componente_lexico = buscar_e_insertar_en_tabla_de_simbolos(componente_lexico->lexema);

    return componente_lexico;
}

ComponenteLexico *aceptar_lexema(int comp) {
    ComponenteLexico *componente_lexico = (ComponenteLexico*) malloc(sizeof(ComponenteLexico));
    if (componente_lexico == NULL) {
        printf("Error al crear el componente lexico\n");
        exit(EXIT_FAILURE);
    }
    componente_lexico->lexema = obtener_lexema();
    componente_lexico->componente_lexico = comp;

    return componente_lexico;
}

ComponenteLexico *retroceder_y_aceptar_lexema(int comp) {
    retroceder_caracter();
    return aceptar_lexema(comp);
}

ComponenteLexico *aceptar_lexema_operadores_y_delimitadores() {
    ComponenteLexico *componente_lexico = (ComponenteLexico*) malloc(sizeof(ComponenteLexico));
    if (componente_lexico == NULL) {
        printf("Error al crear el componente lexico\n");
        exit(EXIT_FAILURE);
    }
    componente_lexico->lexema = obtener_lexema();
    componente_lexico->componente_lexico = comprobar_operadores_y_delimitadores(componente_lexico->lexema);

    return componente_lexico;
}

int comprobar_operadores_y_delimitadores(char *lexema) {
    if (strcmp(lexema, "**") == 0)
        return DOUBLE_STAR;
    else if (strcmp(lexema, "//") == 0)
        return DOUBLE_SLASH;
    else if (strcmp(lexema, "<<") == 0)
        return LEFT_SHIFT;
    else if (strcmp(lexema, ">>") == 0)
        return RIGHT_SHIFT;
    else if (strcmp(lexema, ":=") == 0)
        return ASSIGMENT;
    else if (strcmp(lexema, "<=") == 0)
        return LESS_OR_EQUAL;
    else if (strcmp(lexema, ">=") == 0)
        return GREATER_OR_EQUAL;
    else if (strcmp(lexema, "==") == 0)
        return NOT_EQUAL;
    else if (strcmp(lexema, "!=") == 0)
        return EQUAL_EQUAL;
    else if (strcmp(lexema, "->") == 0)
        return ARROW_OPERATOR;
    else if (strcmp(lexema, "+=") == 0)
        return PLUS_EQUAL;
    else if (strcmp(lexema, "-=") == 0)
        return MINUS_EQUAL;
    else if (strcmp(lexema, "*=") == 0)
        return STAR_EQUAL;
    else if (strcmp(lexema, "/=") == 0)
        return DIVIDE_EQUAL;
    else if (strcmp(lexema, "//=") == 0)
        return FLOOR_DIVIDE_EQUAL;
    else if (strcmp(lexema, "%=") == 0)
        return MODULO_EQUAL;
    else if (strcmp(lexema, "@=") == 0)
        return AT_EQUAL;
    else if (strcmp(lexema, "&=") == 0)
        return AND_EQUAL;
    else if (strcmp(lexema, "|=") == 0)
        return OR_EQUAL;
    else if (strcmp(lexema, "^=") == 0)
        return XOR_EQUAL;
    else if (strcmp(lexema, ">>=") == 0)
        return RIGHT_SHIFT_EQUAL;
    else if (strcmp(lexema, "<<=") == 0)
        return LEFT_SHIFT_EQUAL;
    else if (strcmp(lexema, "**=") == 0)
        return DOUBLE_STAR_EQUAL;
    else {
        gestionar_error("Componente léxico no reconocido", fila, columna);
        return 0;
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

ComponenteLexico *terminar_analisis_lexico() {
    ComponenteLexico *componente_lexico = (ComponenteLexico*) malloc(sizeof(ComponenteLexico));
    if (componente_lexico == NULL) {
        printf("Error al crear el componente lexico\n");
        exit(EXIT_FAILURE);
    }
    componente_lexico->lexema = "Final de fichero";
    componente_lexico->componente_lexico = EOF;
    cerrar_sistema_de_entrada();
    return componente_lexico;
}

