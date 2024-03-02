#ifndef INC_01_LISTA_H
#define INC_01_LISTA_H

#include "definiciones.h"

typedef struct Nodo {
    ComponenteLexico componente_lexico;
    struct Nodo *siguiente;
} Nodo;

typedef struct Lista {
    Nodo *inicio;
} Lista;

Lista *crear_lista();

int buscar(Lista lista, char* lexema);

void insertar(Lista *lista, ComponenteLexico componente_lexico);

void liberar_lista(Lista *lista);

#endif //INC_01_LISTA_H
