#include <stdio.h>
#include <stdlib.h>
#include "dato.h"
#include "tabla.h"


int main() {

    TablaHash tabla = tablahash_crear(sizeof(Dato), dato_copia, dato_compara, dato_destruye, dato_hash);

    Dato dato = dato_crear("1","10");
    tablahash_insertar(tabla, dato);
    dato_destruye(dato);

    dato = dato_crear("2","20");
    tablahash_insertar(tabla, dato);
    dato_destruye(dato);

    dato = dato_crear("1","00");
    void* temp = tablahash_buscar(tabla, dato);

    if (temp != NULL) {
        printf("Devolvio: %s, %d\n", ((Dato)temp)->clave, ((Dato)temp)->valor);
    } else {
        puts("Devolvio NULL");
    }

    tablahash_eliminar(tabla, dato);

    temp = tablahash_buscar(tabla, dato);
    dato_destruye(dato);

    if (temp != NULL) {
        printf("Devolvio: %s, %d\n", ((Dato)temp)->clave, ((Dato)temp)->valor);
    } else {
        puts("Devolvio NULL");
    }

    tablahash_destruir(tabla);

    return 0;
}