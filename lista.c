#include <stdlib.h>
#include "lista.h"
#include <string.h>

Lista *crear_lista() {
    Lista *lista = (Lista*) malloc(sizeof(Lista));
    if (lista != NULL) {
        lista->inicio = NULL;
    }
    return lista;
}

int buscar(Lista lista, char* lexema) {
    Nodo *actual = lista.inicio;
    while(actual != NULL) {
        if (strcmp(actual->componente_lexico.lexema, lexema) == 0)
            return actual->componente_lexico.componente_lexico; // Se encontro el componente lexico
        actual = actual->siguiente;
    }

    return 0;
}


void insertar(Lista *lista, ComponenteLexico componente_lexico) {
    Nodo *nuevo_nodo = (Nodo*) malloc(sizeof(Nodo));
    if (nuevo_nodo != NULL) {
        nuevo_nodo->componente_lexico = componente_lexico;
        nuevo_nodo->siguiente = lista->inicio;
        lista->inicio = nuevo_nodo;
    }
}

void liberar_lista(Lista *lista) {
    Nodo *actual = lista->inicio;
    while (actual != NULL) {
        Nodo *aux = actual;
        actual = actual->siguiente;
        free(aux->componente_lexico.lexema);
        free(aux);
    }
    free(lista);
}


