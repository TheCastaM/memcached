#include <string.h>
#include <stdlib.h>

/**
 * Codigo de las funciones para operar con el diccionario
 */

typedef struct {
    char* clave;
    int valor;
} Dato;

/**
 * Crea un dato clave-valor
 */
Dato* crear_dato(char* clave, char* valor);

/**
 * Copia un dato clave-valor
 */
void* copia_dato(void* dato);

/**
 * Destruye un dato clave-valor
 */
void destruye_dato(void* dato);

/**
 * Compara un dato clave-valor
 */
int comparar_dato(void* dato1, void* dato2);

/**
 * Da un valor en base a un dato clave-valor
 */
int hash_dato(void* dato);