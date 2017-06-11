#include <stdlib.h>
#include <stdbool.h>

#define FACTOR 2

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef int (*cmp_func_t) (const void *a, const void *b);

typedef struct heap {
	void* *datos;
	size_t cantidad;
	size_t tam;
	cmp_func_t cmp;
} heap_t;

/* ******************************************************************
 *                       FUNCIONES AUXILIARES
 * *****************************************************************/

// Compara dos valores y devuelve la posición del valor máximo.
static size_t max(void*x, size_t pos1, void* y, size_t pos2, cmp_func_t cmp) {

	if (cmp(x, y) >= 0) {
		return pos1;
	} else {
		return pos2;
	}
}

// Función downheap.
static void downheap(void* arreglo[], size_t pos, size_t cant, cmp_func_t cmp) {

	size_t i = pos;
	size_t posicion;

	while (i < cant/2) {
		
		if ((2*i + 2) >= cant) {

			posicion = 2*i + 1;

		} else {
			
			posicion = max(arreglo[2*i + 1], 2*i + 1, arreglo[2*i + 2], 2*i + 2, cmp);
		}

		if (cmp(arreglo[i], arreglo[posicion]) < 0) {
			
			void* valor = arreglo[i];
			arreglo[i] = arreglo[posicion];
			arreglo[posicion] = valor;
		}

		if (posicion == 2*i+1) {
			
			i = 2*i + 1;

		} else {
			
			i = 2*i + 2;
		}
	}
}

// Función upheap.
static void upheap(void* arreglo[], size_t pos, size_t cant, cmp_func_t cmp) {

	size_t i = cant;
	
	while (i > 0) {
		
		if (cmp(arreglo[i], arreglo[(i-1)/2]) > 0) {
			
			void* valor = arreglo[i];
			arreglo[i] = arreglo[(i-1)/2];
			arreglo[(i-1)/2] = valor;
		}
		
		i = (i-1)/2;
	}
}

// Función heapify que crea un heap de máximos en este caso.
static void heapify(void* arreglo[], size_t cant, cmp_func_t cmp) {

	for (int i = (cant-2)/2; i >= 0; i--) {

		downheap(arreglo, i, cant, cmp);
	}
}

// Redimensiona el heap.
static bool redimensionar_heap(heap_t *heap, size_t tam_nuevo) {
	
	void* datos_nuevo = realloc (heap->datos, tam_nuevo * sizeof(void*));

	if (!datos_nuevo) return false;

	heap->datos = datos_nuevo;
	heap->tam = tam_nuevo;

	return true;
}

// Redimensiona el heap cuando la proporción entre tamaño del heap y lugar ocupado es del 33%.
static bool redimensionar(heap_t *heap, int proporcion) {

	if (proporcion == 33) {

		return redimensionar_heap(heap, (heap->tam/2));
	}

	return false;
}

/* ******************************************************************
 *                       PRIMITIVAS DEL HEAP
 * *****************************************************************/

heap_t *heap_crear(cmp_func_t cmp, size_t tam) {

	heap_t *heap = malloc(sizeof(heap_t));
	
	if (!heap) return NULL;

	heap->datos = calloc(tam * sizeof(void*), sizeof(void*));
	
	if (!heap->datos) {
		
		free(heap);
		return NULL;
	}
	
	heap->cantidad = 0;
	heap->cmp = cmp;
	heap->tam = tam;
	
	return heap;
}

size_t heap_cantidad(const heap_t *heap) {

	return heap->cantidad;
}

bool heap_esta_vacio(const heap_t *heap) {

	return heap->cantidad == 0;
}

void* heap_ver_max(const heap_t *heap) {

	if (heap_esta_vacio(heap)) return NULL;
	
	return heap->datos[0];
}

bool heap_encolar(heap_t *heap, void *elem) {

	if (heap_esta_vacio(heap)) {

		heap->datos[0] = elem;
		heap->cantidad = 1;
		
		return true;

	} else if (heap->cantidad == heap->tam) {
		
		if (redimensionar_heap(heap, FACTOR * heap->tam)) {
			
			heap->datos[heap->cantidad] = elem;
			upheap(heap->datos, heap->cantidad, heap->cantidad, heap->cmp);
			(heap->cantidad)++;
			
			return true;
		}
		
		return false;

	} else {

		heap->datos[heap->cantidad] = elem;
		upheap(heap->datos, heap->cantidad, heap->cantidad, heap->cmp);
		(heap->cantidad)++;

		return true;
	}
}

void* heap_desencolar(heap_t *heap) {

	if (heap_esta_vacio(heap)) return NULL;

	int proporcion = ((heap->cantidad)/(heap->tam))*100;
	void* valor = heap->datos[0];
	
	if (redimensionar(heap, proporcion)) {
		
		(heap->cantidad)--;
		heap->datos[0] = heap->datos[heap->cantidad];
		
		downheap(heap->datos, 0, heap->cantidad, heap->cmp);
		
		return valor;
	}
	
	(heap->cantidad)--;
	heap->datos[0] = heap->datos[heap->cantidad];
	
	downheap(heap->datos, 0, heap->cantidad, heap->cmp);
	
	return valor;
}

void heap_remover(heap_t* heap) {

	int proporcion = ((heap->cantidad)/(heap->tam))*100;
	
	if (redimensionar(heap, proporcion)) {
		
		(heap->cantidad)--;
		heap->datos[0] = heap->datos[heap->cantidad];
		
		downheap(heap->datos, 0, heap->cantidad, heap->cmp);
		
		return;
	}
	
	(heap->cantidad)--;
	heap->datos[0] = heap->datos[heap->cantidad];
	
	downheap(heap->datos, 0, heap->cantidad, heap->cmp);
	
	return;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *elemento)) {

	void* elemento;
	
	if (destruir_elemento) {
		
		while (!heap_esta_vacio(heap)) {
			
			elemento = heap_desencolar(heap);
			destruir_elemento(elemento);
		}
	}
	
	free(heap->datos);
	free (heap);
}

/* ******************************************************************
 *                            HEAP SORT
 * *****************************************************************/

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp) {

	heapify(elementos, cant, cmp);
	
	for (int i = cant-1; i >= 0; i--) {
		
		void* valor = elementos[i];
		elementos[i] = elementos[0];
		elementos[0] = valor;
		
		downheap(elementos, 0, i, cmp);
	}
}

