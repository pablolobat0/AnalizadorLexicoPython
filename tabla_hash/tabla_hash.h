#ifndef ANALIZADORLEXICOPYTHON_TABLA_HASH_H
#define ANALIZADORLEXICOPYTHON_TABLA_HASH_H

#include <stdbool.h>

typedef struct TablaHash TablaHash;


TablaHash *crear_tabla();

void insertar(TablaHash *tabla_hash, char *key, int value);

int buscar(TablaHash *tabla_hash, char *key);

void destruir_tabla(TablaHash *tabla_hash);

#endif //ANALIZADORLEXICOPYTHON_TABLA_HASH_H