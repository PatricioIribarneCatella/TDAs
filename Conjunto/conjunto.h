#ifndef CONJUNTO_T
#define CONJUNTO_T

#include <stdlib.h>
#include <stdbool.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* Prototipo de función de comparación que se le pasa como parámetro.
 * Debe recibir dos punteros del tipo de dato utilizado en el conjunto, y
 * debe devolver:
 *   menor a 0  si  a < b
 *       0      si  a == b
 *   mayor a 0  si  a > b
 */

typedef int (*cmp_func_t) (const void *a, const void *b);

typedef void (*destruir_dato_t) (void *);

typedef struct conjunto conjunto_t;

/* ******************************************************************
 *                    PRIMITIVAS DEL CONJUNTO
 * *****************************************************************/

/* Crea un conjunto de tamaño tam.
   Devuelve un puntero a NULL en caso de error. */
conjunto_t* conjunto_crear(int tamanio, cmp_func_t cmp, destruir_dato_t destruir_dato);

/* Agrega un dato al conjunto. Devuelve false en caso de error. */
bool conjunto_agregar(conjunto_t* conjunto, void* dato);

/* Elimina un dato del conjunto. */
bool conjunto_eliminar(conjunto_t* conjunto, void* dato);

/* Devuelve verdadero en caso de que el dato pertenezca al conjunto,
   falso en caso contrario. */
bool conjunto_pertenece(conjunto_t* conjunto, void* dato);

/* Devuelve la unión de dos conjuntos en un conjunto nuevo.
   Devueve NULL en caso de error. */
conjunto_t* conjunto_union(conjunto_t* conjunto1, conjunto_t* conjunto2);

/* Devuelve la intersección de dos conjuntos en un conjunto nuevo.
   Devueve NULL en caso de error. */
conjunto_t* conjunto_interseccion(conjunto_t* conjunto1, conjunto_t* conjunto2);

/* Destruye el conjunto*/
void conjunto_destruir (conjunto_t* conjunto);

#endif // CONJUNTO_T