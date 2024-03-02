#include "tabla_de_simbolos.h"
#include "lista.h"

typedef Lista TablaDeSimbolos;

TablaDeSimbolos *tabla_de_simbolos;

void crear_tabla_de_simbolos() {
    tabla_de_simbolos = crear_lista();
    ComponenteLexico palabras_clave[] = {
            {"False", FALSE},
            {"None", NONE},
            {"True", TRUE},
            {"and", AND},
            {"as", AS},
            {"assert", ASSERT},
            {"async", ASYNC},
            {"await", AWAIT},
            {"except", EXCEPT},
            {"finally", FINALLY},
            {"for", FOR},
            {"global", GLOBAL},
            {"if", IF},
            {"import", IMPORT},
            {"in", IN},
            {"is", IS},
            {"lambda", LAMBDA},
            {"nonlocal", NONLOCAL},
            {"not", NOT},
            {"or", OR},
            {"pass", PASS},
            {"raise", RAISE},
            {"return", RETURN},
            {"try", TRY},
            {"while", WHILE},
            {"with", WITH},
            {"yield", YIELD},
    };

    for (int i = 0; i < (sizeof(palabras_clave) / sizeof(ComponenteLexico)); i++) {
        insertar(tabla_de_simbolos, palabras_clave[i]);
    }
}

int buscar_e_insertar_en_tabla_de_simbolos(char *lexema) {
    int comp = buscar(*tabla_de_simbolos, lexema);
    if (comp)
        return comp;
    ComponenteLexico componente_lexico = {lexema, ID};
    insertar(tabla_de_simbolos, componente_lexico);
    return componente_lexico.componente_lexico;
}

void destruir_tabla_de_simbolos() {
    liberar_lista(tabla_de_simbolos);
}

