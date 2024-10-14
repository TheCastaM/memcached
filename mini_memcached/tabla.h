#ifndef __TABLAHASH_H__
#define __TABLAHASH_H__

/** Retorna una copia fisica del dato */
typedef void *(*FuncionCopiadora)(void *dato);

/** Retorna un entero negativo si dato1 < dato2, 0 si son iguales y un entero
 * positivo si dato1 > dato2  */
typedef int (*FuncionComparadora)(void *dato1, void *dato2);

/** Libera la memoria alocada para el dato */
typedef void (*FuncionDestructora)(void *dato);

/** Retorna un entero sin signo para el dato */
typedef unsigned (*FuncionHash)(void *dato);

typedef struct _TablaHash *TablaHash;

/**
 * Crea una nueva tabla hash vacia, con la capacidad dada.
 */
TablaHash tablahash_crear(unsigned capacidad, FuncionCopiadora copia,
                          FuncionComparadora comp, FuncionDestructora destr,
                          FuncionHash hash);

/**
 * Retorna el numero de elementos de la tabla.
 */
int tablahash_nelems(TablaHash tabla);

/**
 * Retorna la capacidad de la tabla.
 */
int tablahash_capacidad(TablaHash tabla);

/**
 * Destruye la tabla.
 */
void tablahash_destruir(TablaHash tabla);

/**
 * Inserta un dato en la tabla, o lo reemplaza si ya se encontraba.
 */
void tablahash_insertar(TablaHash tabla, void *dato);

/**
 * Retorna el dato de la tabla que coincida con el dato dado, o NULL si el dato
 * buscado no se encuentra en la tabla.
 */
void *tablahash_buscar(TablaHash tabla, void *dato);

/**
 * Elimina el dato de la tabla que coincida con el dato dado.
 */
void tablahash_eliminar(TablaHash tabla, void *dato);

/**
 * Rehashea la tabla para que pueda ingresar mas datos.
 */
void tablahash_rehashear(TablaHash tabla);

#endif /* __TABLAHASH_H__ */