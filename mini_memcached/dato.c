#include <string.h>
#include <stdlib.h>
#include "dato.h"

/**
 * Crea un dato clave-valor
 */
Dato* crear_dato(char* clave, char* valor) {
    Dato* dato = malloc(sizeof(Dato*));
    dato->clave = clave;
    dato->valor = atoi(valor);
    return dato;
}

/**
 * Copia un dato clave-valor
 */
void* copia_dato(void* dato) {
    return dato;
}

/**
 * Destruye un dato clave-valor
 */
void destruye_dato(void* dato) {
    free(dato);
}

/**
 * Compara un dato clave-valor
 */
int comparar_dato(void* dato1, void* dato2) {
    strcmp(((Dato*) dato1)->clave,((Dato*) dato2)->clave);
}

/**
 * Da un valor en base a un dato clave-valor
 */
int hash_dato(void* dato) {
    return atoi(((Dato*) dato)->clave);
}