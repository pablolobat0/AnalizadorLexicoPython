#include "analizador_lexico.h"
#include "../lex.yy.h"


void siguiente_componente_lexico(ComponenteLexico *componente_lexico) {
    siguiente_componente_lexico_flex(componente_lexico);
}
