#ifndef HASH_H
#define HASH_H

#include <stdbool.h>
#include <stddef.h>
#include "lista.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct hash hash_t;

typedef struct hash_iter hash_iter_t;

typedef void (*hash_destruir_dato_t)(void *);

typedef size_t (*f_hash_t)(const char* clave, size_t tam);

/* ******************************************************************
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/
// Crea una tabla de Hash.
// Post: devuelve una nueva tabla de Hash.
hash_t *hash_crear(hash_destruir_dato_t destruir_dato, f_hash_t fhash);

// Guarda una nueva clave con su correspodiente valor asociado en el hash.
// Pre: el hash fue creado.
// Post: se guardó correctamente la clave con su valor.
bool hash_guardar(hash_t *hash, const char *clave, void *dato);

// Borra la clave y su correpondiente valor asociado en el hash.
// Pre: el hash fue creado. La clave en el hash existe.
// Post: devuelve el valor asociado a la clave y retira esa clave de la tabla de hash.
void *hash_borrar_dato(hash_t *hash, const char *clave);

// Borra la clave y su correpondiente valor asociado en el hash.
// Pre: el hash fue creado. La clave en el hash existe.
// Post: devuelve true o false dependiendo de si el valor fue distinto de NULL y retira esa clave de la tabla de hash.
bool hash_borrar(hash_t *hash, const char *clave);

// Obtiene el valor asociado a una clave.
// Pre: el hash fue creado. La clave existe en el hash.
// Post: devuelve el valor de la clave, sin retirar la clave del hash.
void *hash_obtener(const hash_t *hash, const char *clave);

// Verfica si una clave pertenece o no a una tabla de hash.
// Pre: el hash fue creado.
// Post: devuelve verdadero o falso dependiendo de si la clave se encontraba o no en el hash.
bool hash_pertenece(const hash_t *hash, const char *clave);

// Devuelve el número de elementos que se encuentran en el hash.
// Pre: el hash fue creado.
// Post: devuelve un número positivo indicando la cantidad de elementos guardados en el hash.
size_t hash_cantidad(const hash_t *hash);

// Devuelve una lista con todas las claves del hash.
lista_t* hash_claves(const hash_t* hash);

// Destruye la tabla de hash.
// Pre: el hash fue creado.
// Post: destruye el hash y todos los elementos que contenía.
void hash_destruir(hash_t *hash);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

// Crea un nuevo iterador de hash.
// Pre: el hash fue creado.
// Post: devuelve un nuevo iterador de hash.
hash_iter_t *hash_iter_crear(const hash_t *hash);

// Avanza sobre el hash de un elemento a la vez.
// Pre: el hash y el iterador fueron creados.
// Post: se movió una posición en el hash, devuelve verdadero si lo pudo concretar o falso en caso contrario.
bool hash_iter_avanzar(hash_iter_t *iter);

// Devuelve la clave a la cual esta apuntando el iterador en ese momento.
// Pre: el hash y el iterador fueron creados.
// Post: devuelve la clave actual.
const char *hash_iter_ver_actual(const hash_iter_t *iter);

// Se fija si el iterador está al final del hash.
// Pre: el hash y el iterador fueron creados.
// Post: devuelve verdadero en caso de que el iterador se encuentre al final del hash o falso en caso contrario.
bool hash_iter_al_final(const hash_iter_t *iter);

// Destruye el iterador.
// Pre: el hash y el iterador fueron creados.
// Post: el iterador  fue destruido.
void hash_iter_destruir(hash_iter_t* iter);

#endif // HASH_H
