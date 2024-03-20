#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "analizador_lexico.h"
#include "../sistema_de_entrada/sistema_entrada.h"
#include "../gestion_de_errores/gestion_de_errores.h"

// Estados de los automatas
#define DIGITO_ENCONTRADO 1
#define GUION_BAJO_ENCONTRADO 2
#define NO_PUEDE_HABER_OTRO_GUION_BAJO 3
#define HAY_CARACTER_DE_ESCAPE 4
#define NO_HAY_CARACTER_DE_ESCAPE 5
#define NO_HAY_COMILLAS 6
#define HAY_UNA_COMILLA 7
#define HAY_DOS_COMILLAS 8

int fila = 1, columna = 1; // Indican la posición del fichero fuente en la que estamos
bool introducir_en_tabla_de_simoblos;
bool tam_maximo_de_lexema_excedido = false;
bool enviar_al_analizador_sintactico;
ComponenteLexico *componente_lexico;

bool es_hexadecimal(int caracter);

bool es_posible_operador_delimitador_doble(int caracter);

bool es_posible_delimitador_triple(int caracter);

bool es_simbolo_simple(int caracter);

ComponenteLexico *automata_alfanumerico();

ComponenteLexico *automata_numeros_que_empiezan_por_cero();

ComponenteLexico *automata_binario();

ComponenteLexico *automata_octal();

ComponenteLexico *automata_hexadecimal();

ComponenteLexico *automata_numeros_que_no_empiezan_por_cero();

ComponenteLexico *automata_exponente();

ComponenteLexico *automata_punto_flotante();

ComponenteLexico *automata_entero(int estado);

int automata_numero(int estado);

bool comprobar_si_acaba_en_guion_bajo(int estado);

ComponenteLexico *automata_delimitadores_triples(int caracter_actual);

ComponenteLexico *automata_operadores_delimitadores_dobles(int caracter_actual);

ComponenteLexico *automata_string(int comillas);

void automata_string_triples_comillas();

ComponenteLexico *retroceder_y_aceptar_lexema(int comp);

ComponenteLexico *aceptar_lexema_y_buscar_e_insertar_en_tabla_de_simbolos();

ComponenteLexico *aceptar_lexema(int comp);

ComponenteLexico *aceptar_lexema_que_ha_excedido_el_tamano_maximo();

void saltar_linea();

ComponenteLexico *terminar_analisis_lexico();


void iniciar_analizador_lexico() {
    componente_lexico = (ComponenteLexico *) malloc(sizeof(ComponenteLexico));
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
        } else if (caracter_actual == '0') {
            return automata_numeros_que_empiezan_por_cero();
        } else if (isdigit(caracter_actual)) {
            return automata_numeros_que_no_empiezan_por_cero();
        } else if (caracter_actual == 32 || caracter_actual == '\t') {
            saltar_caracter();
        } else if (caracter_actual == '\n') {
            fila++;
            columna = 0;
            saltar_caracter();
        } else if (es_simbolo_simple(caracter_actual)) {
            return aceptar_lexema(caracter_actual); // Cuando es un simbolo se devuelve el ASCII
        } else if (es_posible_operador_delimitador_doble(caracter_actual)) {
            return automata_operadores_delimitadores_dobles(caracter_actual);
        } else if (es_posible_delimitador_triple(caracter_actual)) {
            return automata_delimitadores_triples(caracter_actual);
        } else if (caracter_actual == '.') {
            caracter_actual = siguiente_caracter();
            if (isdigit(caracter_actual)) {
                return automata_punto_flotante();
            } else {
                retroceder_caracter();
                return aceptar_lexema(caracter_actual);
            }
        } else if (caracter_actual == '\'') {
            return automata_string(caracter_actual);
        } else if (caracter_actual == '\"') {
            caracter_actual = siguiente_caracter();
            if (caracter_actual != '\"')
                return automata_string('\"');
            caracter_actual = siguiente_caracter();
            if (caracter_actual == '\"') {
                enviar_al_analizador_sintactico = false;
                automata_string_triples_comillas();
            }
            else
                return retroceder_y_aceptar_lexema(STRING);
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

ComponenteLexico *automata_numeros_que_empiezan_por_cero() {
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
            return retroceder_y_aceptar_lexema(INT);
    }
}

ComponenteLexico *automata_binario() {
    int caracter_actual;
    int estado = NO_PUEDE_HABER_OTRO_GUION_BAJO;

    while (true) {
        caracter_actual = siguiente_caracter();
        if (caracter_actual == '1' || caracter_actual == '0') {
            estado = DIGITO_ENCONTRADO;
        } else if (caracter_actual == '_' && estado == DIGITO_ENCONTRADO) {
            estado = GUION_BAJO_ENCONTRADO;
        } else {
            break;
        }
    }

    if (comprobar_si_acaba_en_guion_bajo(estado)) { // Se retrocede 2 veces, un número no puede acabar en '_'
        retroceder_caracter();
    }

    return retroceder_y_aceptar_lexema(INT);
}

ComponenteLexico *automata_octal() {
    int caracter_actual;
    int estado = NO_PUEDE_HABER_OTRO_GUION_BAJO;

    while (true) {
        caracter_actual = siguiente_caracter();
        if (caracter_actual >= '0' && caracter_actual <= '7') {
            estado = DIGITO_ENCONTRADO;
        } else if (caracter_actual == '_' && estado == DIGITO_ENCONTRADO) {
            estado = GUION_BAJO_ENCONTRADO;
        } else {
            break;
        }
    }

    if (comprobar_si_acaba_en_guion_bajo(estado)) {
        retroceder_caracter();
    }

    return retroceder_y_aceptar_lexema(INT);
}

ComponenteLexico *automata_hexadecimal() {
    int caracter_actual;
    int estado = NO_PUEDE_HABER_OTRO_GUION_BAJO;

    while (true) {
        caracter_actual = siguiente_caracter();
        if (es_hexadecimal(caracter_actual)) {
            estado = DIGITO_ENCONTRADO;
        } else if (caracter_actual == '_' && estado == DIGITO_ENCONTRADO) {
            estado = GUION_BAJO_ENCONTRADO;
        } else {
            break;
        }
    }

    if (comprobar_si_acaba_en_guion_bajo(estado)) {
        retroceder_caracter();
    }

    return retroceder_y_aceptar_lexema(INT);
}

bool es_hexadecimal(int caracter) {
    return ((caracter >= '0' && caracter <= '9') || (caracter >= 'A' && caracter <= 'F') ||
            (caracter >= 'a' && caracter <= 'f'));
}

ComponenteLexico *automata_numeros_que_no_empiezan_por_cero() {
    int caracter_actual = siguiente_caracter();
    columna++;
    switch (caracter_actual) { // Clasificamos los numero segun su 2 caracter
        case 'e':
        case 'E':
            return automata_exponente();
        case '.':
            return automata_punto_flotante();
        default:
            if (isdigit(caracter_actual))
                return automata_entero(DIGITO_ENCONTRADO);
            else if(caracter_actual == '_')
                return automata_entero(GUION_BAJO_ENCONTRADO);
    }
    return retroceder_y_aceptar_lexema(INT);
}

ComponenteLexico *automata_exponente() {
    int ultimo_caracter = automata_numero(NO_PUEDE_HABER_OTRO_GUION_BAJO);
    if (ultimo_caracter == '-')
        ultimo_caracter = automata_numero(GUION_BAJO_ENCONTRADO);
    if (ultimo_caracter == 'j' || ultimo_caracter == 'J')
        return aceptar_lexema(IMAGINARY);

    return retroceder_y_aceptar_lexema(FLOAT);
}

ComponenteLexico *automata_punto_flotante() {
    int ultimo_caracter = automata_numero(NO_PUEDE_HABER_OTRO_GUION_BAJO);
    if (ultimo_caracter == 'e' || ultimo_caracter == 'E')
        return automata_exponente();
    else if (ultimo_caracter == 'j' || ultimo_caracter == 'J')
        return aceptar_lexema(IMAGINARY);

    return retroceder_y_aceptar_lexema(FLOAT);
}

ComponenteLexico *automata_entero(int estado) {
    int ultimo_caracter = automata_numero(estado);

    if (ultimo_caracter == '.')
        return automata_punto_flotante();
    else if (ultimo_caracter == 'j' || ultimo_caracter == 'J')
        return aceptar_lexema(IMAGINARY);

    return retroceder_y_aceptar_lexema(INT);
}

int automata_numero(int estado) {
    int caracter_actual;
    while (true) {
        caracter_actual = siguiente_caracter();
        columna++;
        if (isdigit(caracter_actual)) {
            estado = DIGITO_ENCONTRADO;
        } else if (caracter_actual == '_' && estado == DIGITO_ENCONTRADO) {
            estado = GUION_BAJO_ENCONTRADO;
        } else {
            break;
        }
    }

    if (comprobar_si_acaba_en_guion_bajo(estado)) { // Se retrocede 2 veces, un número no puede acabar en '_'
        retroceder_caracter();
        return -1;
    }

    return caracter_actual;
}

bool comprobar_si_acaba_en_guion_bajo(int estado) {
    if (estado == GUION_BAJO_ENCONTRADO) {
        return true;
    }
    return false;
}

// Entiendase simple como los que tienen la forma '{' solo
bool es_simbolo_simple(int caracter) {
    return (caracter == ';' || caracter == ',' || caracter == '[' || caracter == ']' || caracter == '(' ||
            caracter == ')' || caracter == '{' || caracter == '}' || caracter == '~');
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
    return (caracter == ':' || caracter == '=' || caracter == '!' || caracter == '+' || caracter == '-' ||
            caracter == '%' || caracter == '@' || caracter == '&' || caracter == '|' || caracter == '^');
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
                if (caracter_actual == '=') {
                    return aceptar_lexema(MINUS_EQUAL);
                } else if (caracter_actual == '>') {
                    return aceptar_lexema(ARROW_OPERATOR);
                } else {
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

ComponenteLexico *automata_string(int comillas) {
    int caracter_actual;
    int estado = NO_HAY_CARACTER_DE_ESCAPE;
    while (true) {
        caracter_actual = siguiente_caracter();
        columna++;
        switch (caracter_actual) {
            case '\n':
                fila++;
                columna = 1;
                break;
            case '\\':
                estado = HAY_CARACTER_DE_ESCAPE;
                break;
            default:
                if (caracter_actual == comillas && estado != HAY_CARACTER_DE_ESCAPE)
                    return aceptar_lexema(STRING);
                else if (caracter_actual == EOF) {
                    lanzar_error(ERR_STRING_LITERAL_MAL_FORMADO, fila, columna);
                    return terminar_analisis_lexico();
                }
                else
                    estado = NO_HAY_CARACTER_DE_ESCAPE;
        }
    }
}

void automata_string_triples_comillas() {
    int caracter_actual;
    int estado = NO_HAY_COMILLAS;
    int caracter_escape = NO_HAY_CARACTER_DE_ESCAPE;
    while (true) {
        caracter_actual = siguiente_caracter();
        switch (estado) {
            case NO_HAY_COMILLAS:
                if(caracter_actual == '\"' && caracter_escape == NO_HAY_CARACTER_DE_ESCAPE)
                    estado = HAY_UNA_COMILLA;
                else if (caracter_actual == '\\') {
                    caracter_escape = HAY_CARACTER_DE_ESCAPE;
                } else if (caracter_actual == EOF) {
                    lanzar_error(ERR_STRING_LITERAL_MAL_FORMADO, fila, columna);
                    retroceder_caracter();
                    return;
                } else
                    caracter_escape = NO_HAY_CARACTER_DE_ESCAPE;
                break;
            case HAY_UNA_COMILLA:
                if(caracter_actual == '\"' && caracter_escape == NO_HAY_CARACTER_DE_ESCAPE)
                    estado = HAY_DOS_COMILLAS;
                else if (caracter_actual == '\\'){
                    caracter_escape = HAY_CARACTER_DE_ESCAPE;
                } else if (caracter_actual == EOF) {
                    lanzar_error(ERR_STRING_LITERAL_MAL_FORMADO, fila, columna);
                    retroceder_caracter();
                    return;
                } else {
                    caracter_escape = NO_HAY_CARACTER_DE_ESCAPE;
                    estado = NO_HAY_COMILLAS;
                }
                break;
            case HAY_DOS_COMILLAS:
                if(caracter_actual == '\"')
                    return;
                else if (caracter_actual == EOF) {
                    lanzar_error(ERR_STRING_LITERAL_MAL_FORMADO, fila, columna);
                    retroceder_caracter();
                    return;
                }
                else {
                    lanzar_error(ERR_STRING_LITERAL_MAL_FORMADO, fila, columna);
                    return;
                }
        }
    }
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
    return componente_lexico;
}
