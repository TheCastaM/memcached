#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dato.h"

/**
 * Crea un dato clave-valor
 */
Dato dato_crear(char* clave, char* valor) {
    Dato dato = malloc(sizeof(struct _Dato));
    dato->clave = clave;
    dato->valor = atoi(valor);
    return dato;
}

/**
 * Copia un dato clave-valor
 */
void* dato_copia(void* dato) {
    Dato new = malloc(sizeof(struct _Dato));
    new->clave = ((Dato)dato)->clave;
    new->valor = ((Dato)dato)->valor;
    return new;
}

/**
 * Destruye un dato clave-valor
 */
void dato_destruye(void* dato) {
    free(dato);
}

/**
 * Compara un dato clave-valor
 */
int dato_compara(void* dato1, void* dato2) {
    return strcmp(((Dato)dato1)->clave,((Dato)dato2)->clave);
}

/**
 * Da un valor en base a un dato clave-valor
 */
unsigned dato_hash(void* dato) {
    return abs(atoi(((Dato)dato)->clave));
}