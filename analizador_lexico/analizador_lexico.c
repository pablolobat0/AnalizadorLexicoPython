#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "analizador_lexico.h"
#include "../sistema_de_entrada/sistema_entrada.h"
#include "../gestion_de_errores/gestion_de_errores.h"

int fila = 1, columna = 1;
bool introducir_en_tabla_de_simoblos;
bool tam_maximo_de_lexema_excedido = false;
bool enviar_al_analizador_sintactico;
ComponenteLexico *componente_lexico;

bool es_hexadecimal(int caracter);
bool es_posible_operador_delimitador_doble(int caracter);
bool es_posible_delimitador_triple(int caracter);
bool es_simbolo_simple(int caracter);
bool tiene_comillas_triples(int caracter_inicial);
ComponenteLexico *automata_alfanumerico();
int automata_numero();
ComponenteLexico *automata_exponente();
ComponenteLexico *automata_punto_flotante();
ComponenteLexico *automata_binario();
ComponenteLexico *automata_octal();
ComponenteLexico *automata_hexadecimal();
ComponenteLexico *automata_entero();
ComponenteLexico *automata_numerico(int caracter_actual);
ComponenteLexico *automata_simbolos(int caracter_actual);
ComponenteLexico *automata_delimitadores_triples(int caracter_actual);
ComponenteLexico *automata_operadores_delimitadores_dobles(int caracter_actual);
ComponenteLexico *automata_string_triple_comilla();
ComponenteLexico *automata_string(int caracter_inicial);
ComponenteLexico *retroceder_y_aceptar_lexema(int comp);
ComponenteLexico *aceptar_lexema_y_buscar_e_insertar_en_tabla_de_simbolos();
ComponenteLexico *aceptar_lexema(int comp);
ComponenteLexico *aceptar_lexema_que_ha_excedido_el_tamano_maximo();
void saltar_linea();
ComponenteLexico *terminar_analisis_lexico();


void iniciar_analizador_lexico() {
    componente_lexico = (ComponenteLexico*) malloc(sizeof(ComponenteLexico));
    if (componente_lexico == NULL) {
        printf("Error al crear el componente lexico\n");
        exit(EXIT_FAILURE);
    }
}


ComponenteLexico *siguiente_componente_lexico() {
    introducir_en_tabla_de_simoblos = false;
    enviar_al_analizador_sintactico = true;
    while (true) {
        int caracter_actual = siguiente_caracter();
        columna++;
        if (isalpha(caracter_actual) || caracter_actual == '_') {
            introducir_en_tabla_de_simoblos = true;
            return automata_alfanumerico();
        } else if (isdigit(caracter_actual)) {
            return automata_numerico(caracter_actual);
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
            enviar_al_analizador_sintactico = false;
            saltar_linea();
        } else if (caracter_actual == EOF) {
            return terminar_analisis_lexico();
        } else {
            lanzar_error(ERR_CARACTER_NO_RECONOCIDO, fila, columna);
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

ComponenteLexico *automata_numerico(int caracter_actual) {
    if (caracter_actual == '0') {
        caracter_actual = siguiente_caracter();
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
                return retroceder_y_aceptar_lexema(INT);
        }
    } else {
        caracter_actual = siguiente_caracter();
        columna++;
        switch (caracter_actual) { // Clasificamos los numero segun su 2 caracter
            case 'e':
            case 'E':
                return automata_exponente();
            case '.':
                return automata_punto_flotante();
            default:
                if (isdigit(caracter_actual))
                    return automata_entero();
        }
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
            caracter == ')' || caracter == '{' || caracter == '}' || caracter == '~');
}

ComponenteLexico *automata_simbolos(int caracter_actual) {
    if (es_posible_delimitador_triple(caracter_actual)) {
        return automata_delimitadores_triples(caracter_actual);
    } else if (es_posible_operador_delimitador_doble(caracter_actual)) {
        return automata_operadores_delimitadores_dobles(caracter_actual);
    } else { // Es un '.', caso a parte porque puede ser un float
        caracter_actual = siguiente_caracter();
        if (isdigit(caracter_actual)) {
            return automata_punto_flotante();
        } else {
            retroceder_caracter();
            return aceptar_lexema(caracter_actual);
        }
    }
}

// Entiendase triple los que pueden ser de la forma '*', '*=', '**', o '**='
bool es_posible_delimitador_triple(int caracter) {
    return (caracter == '/' || caracter == '*' || caracter == '<' || caracter == '>');
}

ComponenteLexico *automata_delimitadores_triples(int caracter_actual) {
    int estado = 0;
    switch (caracter_actual) {
        case '/':
            estado = 1;
            break;
        case '*':
            estado = 2;
            break;
        case '<':
            estado = 3;
            break;
        case '>':
            estado = 4;
            break;
        default:
            lanzar_error(ERR_CARACTER_NO_RECONOCIDO, fila, columna);
            saltar_caracter();
    }
    while (true) {
        caracter_actual = siguiente_caracter();
        switch (estado) {
            case 1:
                if (caracter_actual == '=')
                    return aceptar_lexema(DIVIDE_EQUAL);
                else if (caracter_actual == '/')
                    estado = 5;
                else
                    return retroceder_y_aceptar_lexema('/');
                break;
            case 2:
                if (caracter_actual == '=')
                    return aceptar_lexema(STAR_EQUAL);
                else if (caracter_actual == '*')
                    estado = 6;
                else
                    return retroceder_y_aceptar_lexema('*');
                break;
            case 3:
                if (caracter_actual == '=')
                    return aceptar_lexema(LESS_OR_EQUAL);
                else if (caracter_actual == '<')
                    estado = 7;
                else
                    return retroceder_y_aceptar_lexema('<');
                break;
            case 4:
                if (caracter_actual == '=')
                    return aceptar_lexema(GREATER_OR_EQUAL);
                else if (caracter_actual == '>')
                    estado = 8;
                else
                    return retroceder_y_aceptar_lexema('>');
                break;
            case 5:
                if (caracter_actual == '=')
                    return aceptar_lexema(FLOOR_DIVIDE_EQUAL);
                else
                    return retroceder_y_aceptar_lexema(DOUBLE_SLASH);
            case 6:
                if (caracter_actual == '=')
                    return aceptar_lexema(DOUBLE_STAR_EQUAL);
                else
                    return retroceder_y_aceptar_lexema(DOUBLE_STAR);
            case 7:
                if (caracter_actual == '=')
                    return aceptar_lexema(LEFT_SHIFT_EQUAL);
                else
                    return retroceder_y_aceptar_lexema(LEFT_SHIFT);
            case 8:
                if (caracter_actual == '=')
                    return aceptar_lexema(RIGHT_SHIFT_EQUAL);
                else
                    return retroceder_y_aceptar_lexema(RIGHT_SHIFT);
            default:
                lanzar_error(ERR_CARACTER_NO_RECONOCIDO, fila, columna);
                saltar_caracter();
        }
    }
}

// Entiendase doble los que pueden ser de la forma '&' o '&='
bool es_posible_operador_delimitador_doble(int caracter) {
    return (caracter == ':' || caracter == '=' || caracter == '!' || caracter == '+' || caracter == '-' || caracter == '%' ||
            caracter == '@' || caracter == '&' || caracter == '|' || caracter == '^');
}

ComponenteLexico *automata_operadores_delimitadores_dobles(int caracter_actual) {
    int estado = 0;
    switch (caracter_actual) {
        case ':':
            estado = 1;
            break;
        case '=':
            estado = 2;
            break;
        case '!':
            estado = 3;
            break;
        case '+':
            estado = 4;
            break;
        case '-':
            estado = 5;
            break;
        case '%':
            estado = 6;
            break;
        case '@':
            estado = 7;
            break;
        case '&':
            estado = 8;
            break;
        case '|':
            estado = 9;
            break;
        case '^':
            estado = 10;
            break;
        default:
            lanzar_error(ERR_CARACTER_NO_RECONOCIDO, fila, columna);
            saltar_caracter();
    }

    while (true) {
        caracter_actual = siguiente_caracter();
        switch (estado) {
            case 1:
                if (caracter_actual == '=')
                    return aceptar_lexema(ASSIGMENT);
                else
                    return retroceder_y_aceptar_lexema(':');
            case 2:
                if (caracter_actual == '=')
                    return aceptar_lexema(EQUAL_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('=');
            case 3:
                if (caracter_actual == '=')
                    return aceptar_lexema(NOT_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('!');
            case 4:
                if (caracter_actual == '=')
                    return aceptar_lexema(PLUS_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('+');
            case 5:
                if (caracter_actual == '=' ) {
                    return aceptar_lexema(MINUS_EQUAL);
                }
                else if (caracter_actual == '>') {
                    return aceptar_lexema(ARROW_OPERATOR);
                }
                else {
                    retroceder_caracter();
                    return aceptar_lexema('-');
                }
            case 6:
                if (caracter_actual == '=')
                    return aceptar_lexema(MODULO_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('%');
            case 7:
                if (caracter_actual == '=')
                    return aceptar_lexema(AT_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('@');
            case 8:
                if (caracter_actual == '=')
                    return aceptar_lexema(AND_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('&');
            case 9:
                if (caracter_actual == '=')
                    return aceptar_lexema(OR_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('|');
            case 10:
                if (caracter_actual == '=')
                    return aceptar_lexema(XOR_EQUAL);
                else
                    return retroceder_y_aceptar_lexema('^');
            default:
                lanzar_error(ERR_CARACTER_NO_RECONOCIDO, fila, columna);
                saltar_caracter();
        }
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

ComponenteLexico *retroceder_y_aceptar_lexema(int comp) {
    retroceder_caracter();
    return aceptar_lexema(comp);
}

ComponenteLexico *aceptar_lexema_y_buscar_e_insertar_en_tabla_de_simbolos() {
    if (tam_maximo_de_lexema_excedido) {
        return aceptar_lexema_que_ha_excedido_el_tamano_maximo();
    }

    componente_lexico->lexema = obtener_lexema();
    componente_lexico->componente_lexico = buscar_e_insertar_en_tabla_de_simbolos(componente_lexico->lexema);

    return componente_lexico;
}

ComponenteLexico *aceptar_lexema(int comp) {
    componente_lexico->componente_lexico = comp;
    if (tam_maximo_de_lexema_excedido) {
        return aceptar_lexema_que_ha_excedido_el_tamano_maximo();
    }
    componente_lexico->lexema = obtener_lexema();

    return componente_lexico;
}

ComponenteLexico *aceptar_lexema_que_ha_excedido_el_tamano_maximo() {
    saltar_caracter();
    tam_maximo_de_lexema_excedido = false;
    informar_lexema_mayor_tamano_maximo_procesado();
    return componente_lexico;
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

void informar_superacion_tamano_maximo_lexema() {
    if (enviar_al_analizador_sintactico) {
        lanzar_error(ERR_TAMANO_MAXIMO_DE_LEXEMA_EXCEDIDO, 0, 0);
        componente_lexico->lexema = obtener_lexema();
        tam_maximo_de_lexema_excedido = true;
        if (introducir_en_tabla_de_simoblos) {
            componente_lexico->componente_lexico = ID;
            buscar_e_insertar_en_tabla_de_simbolos(componente_lexico->lexema);
        }
    }
}

ComponenteLexico *terminar_analisis_lexico() {
    componente_lexico->lexema = "Final de fichero";
    componente_lexico->componente_lexico = EOF;
    cerrar_sistema_de_entrada();
    return componente_lexico;
}
