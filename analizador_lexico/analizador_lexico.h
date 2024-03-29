#ifndef INC_01_ANALIZADOR_LEXICO_H
#define INC_01_ANALIZADOR_LEXICO_H

#include "../tabla_de_simbolos/definiciones.h"
#include "../tabla_de_simbolos/tabla_de_simbolos.h"

void iniciar_analizador_lexico();

ComponenteLexico *siguiente_componente_lexico();

void informar_superacion_tamano_maximo_lexema();

int get_fila();

int get_columna();

#endif //INC_01_ANALIZADOR_LEXICO_H
