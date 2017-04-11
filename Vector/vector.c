#include <stdlib.h>
#include <stdbool.h>

/* ******************************************************************
 *                 DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef void (*vector_destruir_dato_t) (void *);

typedef struct vector {
	size_t tam;
	void* *datos;
	size_t cant_elem;
	vector_destruir_dato_t destruir_dato;
} vector_t;

/* ******************************************************************
 *                        FUNCIÓN AUXILIAR
 * *****************************************************************/

static bool indiceValido(const vector_t* vector, size_t pos) {

	return (pos < vector->tam && pos >= 0);
}

/* ******************************************************************
 *                     PRIMITIVAS DEL VECTOR
 * *****************************************************************/

vector_t* vector_crear(size_t tam, vector_destruir_dato_t destruir_dato) {

	vector_t* vector = malloc(sizeof(vector_t));
	
	if (vector == NULL) return NULL;

	vector->datos = malloc(tam * sizeof(void*));

	if (vector->datos == NULL) {

		free(vector);
		return NULL;
	}

	vector->tam = tam;
	vector->cant_elem = 0;
	vector->destruir_dato = destruir_dato;

	return vector;
}

bool vector_redimensionar(vector_t *vector, size_t tam_nuevo) {

	void* datos_nuevo = realloc(vector->datos, tam_nuevo * sizeof(void*));
	
	if (tam_nuevo > 0 && datos_nuevo == NULL) {
		return false;
	}
	
	vector->datos = datos_nuevo;
	vector->tam = tam_nuevo;
	
	return true;
}

void* vector_obtener_dato(vector_t *vector, size_t pos) {

	if (!indiceValido(vector, pos)) return NULL;

	(vector->cant_elem)--;

	return vector->datos[pos];
}

void* vector_obtener(const vector_t *vector, size_t pos) {

	if (!indiceValido(vector, pos)) return NULL;
	
	return vector->datos[pos];
}

bool vector_guardar(vector_t *vector, size_t pos, void* valor) {

	if (!indiceValido(vector, pos)) return false;
	
	vector->datos[pos] = valor;
	(vector->cant_elem)++;
	
	return true;
}

size_t vector_obtener_tamanio(vector_t *vector) {

	return vector->tam;
}

size_t vector_obtener_cantidad(vector_t *vector) {

	return vector->cant_elem;
}

void vector_destruir(vector_t *vector) {

	if (vector->destruir_dato) {

		for (int i = 0; i < vector->cant_elem; i++) {
			vector->destruir_dato(vector->datos[i]);
		}
	}

	free(vector->datos);
	free(vector);
}
