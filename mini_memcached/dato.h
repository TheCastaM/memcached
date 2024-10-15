#include <string.h>
#include <stdlib.h>

/**
 * Codigo de las funciones para operar con el diccionario
 */

struct _Dato {
    char* clave;
    int valor;
};

typedef struct _Dato *Dato;

/**
 * Crea un dato clave-valor
 */
Dato dato_crear(char* clave, char* valor);

/**
 * Copia un dato clave-valor
 */
void* dato_copia(void* dato);

/**
 * Destruye un dato clave-valor
 */
void dato_destruye(void* dato);

/**
 * Compara un dato clave-valor
 */
int dato_compara(void* dato1, void* dato2);

/**
 * Da un valor en base a un dato clave-valor
 */
unsigned dato_hash(void* dato);