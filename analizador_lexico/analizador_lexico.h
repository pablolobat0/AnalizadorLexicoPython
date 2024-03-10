#ifndef INC_01_ANALIZADOR_LEXICO_H
#define INC_01_ANALIZADOR_LEXICO_H

#include "../tabla_de_simbolos/definiciones.h"
#include "../tabla_de_simbolos/tabla_de_simbolos.h"

void iniciar_analizador_lexico(char *nombre_archivo);

ComponenteLexico siguiente_componente_lexico();

#endif //INC_01_ANALIZADOR_LEXICO_H
