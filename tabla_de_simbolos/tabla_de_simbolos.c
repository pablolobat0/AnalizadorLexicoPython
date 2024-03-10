#include "tabla_de_simbolos.h"
#include "../tabla_hash/tabla_hash.h"

typedef TablaHash TablaDeSimbolos;

TablaDeSimbolos *tabla_de_simbolos;

void crear_tabla_de_simbolos() {
    tabla_de_simbolos = crear_tabla();

    ComponenteLexico palabras_clave[] = {
            {"False",    FALSE},
            {"None",     NONE},
            {"True",     TRUE},
            {"and",      AND},
            {"as",       AS},
            {"assert",   ASSERT},
            {"async",    ASYNC},
            {"await",    AWAIT},
            {"except",   EXCEPT},
            {"finally",  FINALLY},
            {"for",      FOR},
            {"from",     FROM},
            {"global",   GLOBAL},
            {"if",       IF},
            {"import",   IMPORT},
            {"in",       IN},
            {"is",       IS},
            {"lambda",   LAMBDA},
            {"nonlocal", NONLOCAL},
            {"not",      NOT},
            {"or",       OR},
            {"pass",     PASS},
            {"raise",    RAISE},
            {"return",   RETURN},
            {"try",      TRY},
            {"while",    WHILE},
            {"with",     WITH},
            {"yield",    YIELD},
    };

    for (int i = 0; i < (sizeof(palabras_clave) / sizeof(ComponenteLexico)); i++) {
        insertar(tabla_de_simbolos, palabras_clave[i].lexema, palabras_clave[i].componente_lexico);
    }
}

int buscar_e_insertar_en_tabla_de_simbolos(char *lexema) {
    int comp = buscar(tabla_de_simbolos, lexema);
    if (comp) // Si ya esta en la tabla de simbolos
        return comp;
    // Si no esta, es un identificador

    insertar(tabla_de_simbolos, lexema, ID);
    return ID;
}

void destruir_tabla_de_simbolos() {
    destruir_tabla(tabla_de_simbolos);
}

