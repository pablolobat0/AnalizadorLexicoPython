#include "stdlib.h"
#include <stdio.h>
#include <string.h>

#include "tabla_hash.h"

#define TAMANO_INICIAL_TABLA 137 // Numero primo para el tamano

typedef struct HashItem {
    char *key;
    int value;
    struct HashItem *siguiente;
} HashItem;

struct TablaHash {
    HashItem **items;
    int capacidad, numero_de_items;
};


TablaHash *crear_tabla_con_tamano(int tamano) {
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
    return crear_tabla_con_tamano(TAMANO_INICIAL_TABLA);
}


// Fuente: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
unsigned int funcion_hash(char *key, int tamano) {
    unsigned long hash = 5381; // Numero primo para favorecer redispersion
    for (; *key; ++key) {
        hash ^= *key;
        hash *= 0x5bd1e995; // Factor de mezcla
        hash ^= hash >> 15;
    }
    return hash % tamano;
}

void redispersar(TablaHash *tabla_hash_original) {
    TablaHash *nueva_tabla = crear_tabla_con_tamano((tabla_hash_original->capacidad * 2)); // Doblamos el tamano

    // Trasladamos los elementos de la tabla original a la nueva tabla
    for (int i = 0; i < tabla_hash_original->capacidad; i++) {
        HashItem *item = tabla_hash_original->items[i];
        while (item != NULL) {
            HashItem *aux = item;
            item = aux->siguiente;
            insertar(nueva_tabla, aux->key, aux->value);
            if (aux->key != NULL)
                free(aux->key);
            free(aux);
        }
    }

    // Se asignan los elementos redispersos a la tabla original y se libera la nueva
    free(tabla_hash_original->items);
    tabla_hash_original->items = nueva_tabla->items;
    tabla_hash_original->capacidad = nueva_tabla->capacidad;

    free(nueva_tabla);
}

void insertar(TablaHash *tabla_hash, char *key, int value) {
    unsigned int index = funcion_hash(key, tabla_hash->capacidad);

    if (tabla_hash->numero_de_items >= tabla_hash->capacidad / 2) { // Se redispersa si se excede el factor de carga
        redispersar(tabla_hash);
    }

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

    nuevo_hash_item->value = value;

    if (tabla_hash->items[index] == NULL) { // No hay ningún elemento en la posición
        tabla_hash->items[index] = nuevo_hash_item;
        nuevo_hash_item->siguiente = NULL;
    } else { // Si hay un elemento en la posición se coloca como primero de la lista
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
