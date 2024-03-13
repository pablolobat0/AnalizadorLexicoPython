#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "tabla_hash.h"

#define TAMANO_INICIAL_TABLA 5003

typedef struct HashItem {
    char *key;
    int value;
    struct HashItem *siguiente;
} HashItem;

struct TablaHash {
    HashItem **items;
    int capacidad, numero_de_items;
};


TablaHash *_crear_tabla(int tamano) {
    TablaHash *tabla_hash = (TablaHash *) malloc(sizeof(TablaHash));
    if (tabla_hash == NULL) {
        printf("Error al crear la tabla hash\n");
        exit(EXIT_FAILURE);
    }

    tabla_hash->capacidad = tamano;
    tabla_hash->numero_de_items = 0;
    tabla_hash->items = (HashItem **) calloc(tabla_hash->capacidad, sizeof(HashItem *));
    if (tabla_hash->items == NULL) {
        free(tabla_hash);
        printf("Error al crear los items de la tabla hash\n");
        exit(EXIT_FAILURE);
    }

    return tabla_hash;
}

TablaHash *crear_tabla() {
    return _crear_tabla(TAMANO_INICIAL_TABLA);
}


unsigned int funcion_hash(char *key, int tamano) {
    unsigned int hash = 5381;  // Un número primo inicial

    while (*key) {
        hash = (hash << 5) + *key++;  // Desplazamiento a la izquierda y suma
    }

    return hash % tamano;
}

void redispersar(TablaHash *tabla_hash) {
    TablaHash *nueva_tabla = _crear_tabla((tabla_hash->capacidad * 2)); // Doblamos el tamano

    for (int i = 0; i < tabla_hash->capacidad; i++) {
        HashItem *item = tabla_hash->items[i];
        while (item != NULL) {
            insertar(nueva_tabla, item->key, item->value);
            item = item->siguiente;
        }
    }

    free(tabla_hash->items);
    tabla_hash->items = nueva_tabla->items;
    tabla_hash->capacidad = nueva_tabla->capacidad;

    free(nueva_tabla);
}

void insertar(TablaHash *tabla_hash, char *key, int value) {
    unsigned int index = funcion_hash(key, tabla_hash->capacidad);

    HashItem *nuevo_hash_item = (HashItem *) malloc(sizeof(HashItem));
    if (nuevo_hash_item == NULL) {
        printf("Error en la inserción.\n");
        exit(EXIT_FAILURE);
    }

    nuevo_hash_item->key = strdup(key);  // Copia de la cadena de caracteres
    if (nuevo_hash_item->key == NULL) {
        printf("Error en la inserción.\n");
        free(nuevo_hash_item);
        exit(EXIT_FAILURE);
    }

    if (tabla_hash->numero_de_items >= tabla_hash->capacidad / 2) {
        redispersar(tabla_hash);
    }

    nuevo_hash_item->value = value;

    if (tabla_hash->items[index] == NULL) {
        tabla_hash->items[index] = nuevo_hash_item;
        nuevo_hash_item->siguiente = NULL;
    } else {
        nuevo_hash_item->siguiente = tabla_hash->items[index];
        tabla_hash->items[index] = nuevo_hash_item;
    }

    tabla_hash->numero_de_items++;
}

void imprimir_tabla_hash(TablaHash *tabla_hash) {
    for (int i = 0; i < tabla_hash->capacidad; i++) {
        HashItem *item = tabla_hash->items[i];
        while (item != NULL) {
            printf("%d %s\n", item->value, item->key);
            item = item->siguiente;
        }
    }
}

int buscar_en_lista(HashItem *item, char *key) {
    while (item != NULL) {
        if (strcmp(item->key, key) == 0)
            return item->value;
        item = item->siguiente;
    }

    return false;
}

int buscar(TablaHash *tabla_hash, char *key) {
    unsigned int index = funcion_hash(key, tabla_hash->capacidad);
    return buscar_en_lista(tabla_hash->items[index], key);
}


void destruir_tabla(TablaHash *tabla_hash) {
    for (int i = 0; i < tabla_hash->capacidad; i++) {
        HashItem *item = tabla_hash->items[i];
        while (item != NULL) {
            HashItem *aux = item;
            item = aux->siguiente;
            if (aux->key != NULL)
                free(aux->key);
            free(aux);
        }
    }

    free(tabla_hash->items);
    free(tabla_hash);
}
