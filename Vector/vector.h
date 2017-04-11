#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

/* ******************************************************************
 *                 DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct vector vector_t;

typedef void (*vector_destruir_dato_t)(void *);

/* ******************************************************************
 *                     PRIMITIVAS DEL VECTOR
 * *****************************************************************/

vector_t* vector_crear(size_t tam, vector_destruir_dato_t destruir_dato);

void vector_destruir(vector_t *vector);

bool vector_redimensionar(vector_t *vector, size_t nuevo_tam);

/*Devuelve el dato en la posición especificada y lo retira del vector*/
void* vector_obtener_dato(vector_t* vector, size_t pos);

/*Devuelve el dato en la posición especificada pero no lo retira del vector*/
void* vector_obtener(const vector_t *vector, size_t pos);

bool vector_guardar(vector_t *vector, size_t pos, void* valor);

size_t vector_obtener_tamanio(vector_t *vector);

size_t vector_obtener_cantidad(vector_t* vector);

#endif // VECTOR_H
