#include <stdlib.h>
#include <stdbool.h>

#define FACTOR 2

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef int (*cmp_func_t) (const void *a, const void *b);

typedef void (*destruir_dato_t) (void *);

typedef struct conjunto {
	void* *datos;
	size_t cant;
	size_t tam;
	cmp_func_t cmp;
	destruir_dato_t destruir_dato;
} conjunto_t;

/* ******************************************************************
 *                       FUNCIONES AUXILIARES
 * *****************************************************************/

/* Redimensiona el conjunto. */
static bool redimensionar_conjunto(conjunto_t* conjunto, size_t tam_nuevo) {

	void* datos_nuevo = realloc(conjunto->datos, tam_nuevo * sizeof(void*));

	if (!datos_nuevo) return false;

	conjunto->datos = datos_nuevo;
	conjunto->tam = tam_nuevo;
	
	return true;
}

/* ******************************************************************
 *                    PRIMITIVAS DEL CONJUNTO
 * *****************************************************************/

/* Crea un conjunto de tamaño tam.
   Devuelve un puntero a NULL en caso de error. */
conjunto_t* conjunto_crear(int tamanio, cmp_func_t cmp, destruir_dato_t destruir_dato) {

	if (tamanio <= 0) return NULL;

	conjunto_t* conjunto = malloc(sizeof(conjunto_t));

	if (!conjunto) return NULL;

	void* *datos = malloc(tamanio * sizeof(void*));

	if (!datos) {

		free(conjunto);
		return NULL;
	}

	conjunto->datos = datos;
	conjunto->tam = tamanio;
	conjunto->cant = 0;
	conjunto->cmp = cmp;
	conjunto->destruir_dato = destruir_dato;

	return conjunto;
}

/* Devuelve verdadero en caso de que el dato pertenezca al conjunto,
   falso en caso contrario. */
bool conjunto_pertenece(conjunto_t* conjunto, void* dato) {

	if (!conjunto) return false;
	
	for (size_t i = 0; i < conjunto->cant; i++) {
		
		if (conjunto->cmp(conjunto->datos[i], dato) == 0) return true;
	}
	
	return false;
}

/* Agrega un dato al conjunto. Devuelve false en caso de error. */
bool conjunto_agregar(conjunto_t* conjunto, void* dato) {

	if (!conjunto) return false;
	
	if (conjunto_pertenece(conjunto, dato)) return false;

	if ((conjunto->tam == conjunto->cant) &&
		!redimensionar_conjunto(conjunto, FACTOR * conjunto->tam))
		return false;

	conjunto->datos[conjunto->cant] = dato;
	(conjunto->cant)++;

	return true;
}

/* Elimina un dato del conjunto. */
bool conjunto_eliminar(conjunto_t* conjunto, void* dato) {

	if (!conjunto) return false;

	for (size_t i = 0; i < conjunto->tam; i++) {

		if (conjunto->cmp(conjunto->datos[i], dato) == 0) {

			void* dato = conjunto->datos[i];

			if (conjunto->destruir_dato)
				conjunto->destruir_dato(dato);

			conjunto->datos[i] = conjunto->datos[conjunto->cant-1];
			(conjunto->cant)--;

			return true;
		}

	}

	return false;
}

/* Devuelve la unión de dos conjuntos en un conjunto nuevo.
   Devueve NULL en caso de error. */
conjunto_t* conjunto_union(conjunto_t* conjunto1, conjunto_t* conjunto2) {

	if (!conjunto1 || !conjunto2) return NULL;

	conjunto_t* c_union = conjunto_crear(conjunto1->tam + conjunto2->tam, conjunto1->cmp, conjunto1->destruir_dato);

	if (!c_union) return NULL;

	for (size_t i = 0; i < conjunto1->tam; i++) {
		
		conjunto_agregar(c_union, conjunto1->datos[i]);
	}

	for (size_t i = 0; i < conjunto2->tam; i++) {
		
		if (!conjunto_pertenece(conjunto1, conjunto2->datos[i])) {
			
			conjunto_agregar(c_union, conjunto2->datos[i]);
		}
	}

	return c_union;
}

/* Devuelve la intersección de dos conjuntos en un conjunto nuevo.
   Devueve NULL en caso de error. */
conjunto_t* conjunto_interseccion(conjunto_t* conjunto1, conjunto_t* conjunto2) {

	if (!conjunto1 || !conjunto2) return NULL;

	conjunto_t* c_inter = conjunto_crear(conjunto1->tam, conjunto1->cmp, conjunto1->destruir_dato);
	
	if (!c_inter) return NULL;
	
	for (size_t i = 0; i < conjunto1->tam; i++) {
		
		if (conjunto_pertenece(conjunto2, conjunto1->datos[i])) {
			
			conjunto_agregar(c_inter, conjunto1->datos[i]);
		}
	}

	return c_inter;
}

/* Destruye el conjunto*/
void conjunto_destruir(conjunto_t* conjunto) {

	if (conjunto->destruir_dato) {

		for (size_t i = 0; i < conjunto->cant; i++) {

			conjunto->destruir_dato(conjunto->datos[i]);
		}
	}

	free(conjunto->datos);
	free(conjunto);
}
