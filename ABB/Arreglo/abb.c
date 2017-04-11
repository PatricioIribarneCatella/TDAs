#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pila.h"

#define TAM_INICIAL 100
#define FACTOR 2

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef int (*abb_comparar_clave_t) (const char *, const char *);

typedef void (*abb_destruir_dato_t) (void *);

typedef struct nodo_abb {
	char* clave;
	void* dato;
} nodo_abb_t;

typedef struct abb {
	nodo_abb_t* *datos;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cantidad;
	size_t tamanio;
} abb_t;

typedef struct abb_iter {
	const abb_t* arbol;
	pila_t* pila;
	nodo_abb_t* actual;
} abb_iter_t;

/* ******************************************************************
 *                       FUNCIONES AUXILIARES
 * *****************************************************************/

// Crea un nuevo nodo con clave y valor asociado.
static nodo_abb_t* nodo_crear(const char* clave, void* dato) {

	nodo_abb_t* nodo_nuevo = malloc(sizeof(nodo_abb_t));
	
	if (!nodo_nuevo) return NULL;

	nodo_nuevo->clave = malloc(sizeof(char)*(strlen(clave) + 1));

	if (!nodo_nuevo->clave) {
		free(nodo_nuevo);
		return NULL;
	}

	strcpy(nodo_nuevo->clave, clave);
	nodo_nuevo->dato = dato;
	
	return nodo_nuevo;
}

// Busca la posicin del nodo a borrar en el arreglo.
static size_t busqueda(const abb_t* arbol, const char* clave) {

	abb_comparar_clave_t cmp = arbol->cmp;
	size_t i = 0;
	nodo_abb_t* nodo;
	
	while (i < arbol->tamanio && (arbol->datos[i] != NULL)) {
		
		nodo = arbol->datos[i];
		
		if (cmp(clave, nodo->clave) == 0) return i;

		if (cmp(clave, nodo->clave) > 0) {
			i = 2*i + 2;
		} else {
			i = 2*i + 1;
		}
	}

	return i;
}

// Devuelve la posición dode esta el nodo cuya clave es máxima del subarbol izquierdo.
static size_t maximo(const abb_t* arbol, size_t pos) {

	// No tiene hijo izquierdo, devuelve el derecho.
	nodo_abb_t* nodo = arbol->datos[2*pos + 1];
	if (!nodo) return (2*pos + 2);

	// El hijo izquierdo no tiene hijo derecho, se devuelve éste.
	nodo = arbol->datos[2*(2*pos + 1) + 2];
	if (!nodo) return (2*pos + 1);

	int i = (2*(2*pos + 1) + 2);
	size_t posicion;

	while (nodo != NULL) {
		
		posicion = i;
		i = 2*i + 2;
		nodo = arbol->datos[i];
	}

	return posicion;
}

// Verifica si un nodo es hoja
static bool nodo_es_hoja(const abb_t* arbol, size_t pos) {

	if ((2*pos + 1) > arbol->tamanio) return true;

	return (!arbol->datos[2*pos + 1] && !arbol->datos[2*pos + 2]);
}

// Verifica si un nodo tiene solamente un hijo a derecha y ese es hoja.
static bool nodo_con_un_solo_hijo_derecho(const abb_t* arbol, size_t pos) {

	return (!arbol->datos[2*pos + 1] && arbol->datos[2*pos + 2] && nodo_es_hoja(arbol, 2*pos + 2));
}

// Verifica si un nodo tiene solamente un hijo a izquierda y ese es hoja.
static bool nodo_con_un_solo_hijo_izquierdo(const abb_t* arbol, size_t pos) {

	return (!arbol->datos[2*pos + 2] && arbol->datos[2*pos + 1] && nodo_es_hoja(arbol, 2*pos + 1));
}

// Borra recursivamente un nodo
static void borrar_recursivo(abb_t* arbol, size_t pos) {

	nodo_abb_t* nodo = arbol->datos[pos];

	if (nodo_es_hoja(arbol, pos)) {

		arbol->datos[pos] = NULL;

		free(nodo->clave);
		free(nodo);

		return;

	} else {

		size_t pos_max = maximo(arbol, pos);

		arbol->datos[pos] = arbol->datos[pos_max];
		arbol->datos[pos_max] = nodo;

		borrar_recursivo(arbol, pos_max);
	}
}

// Borra un nodo en particular teniendo en cuenta la cantidad de hijos que posee.
static void* borrar(abb_t* arbol, size_t pos) {

	nodo_abb_t* nodo = arbol->datos[pos];

	void* valor = nodo->dato;

	(arbol->cantidad)--;

	// Nodo sin hijos
	if (nodo_es_hoja(arbol, pos)) {

		arbol->datos[pos] = NULL;
		
		free(nodo->clave);
		free(nodo);
		
		return valor;
	}

	// Nodo con un solo hijo a derecha
	if (nodo_con_un_solo_hijo_derecho(arbol, pos)) {

		nodo = arbol->datos[pos];
		arbol->datos[pos] = arbol->datos[2*pos + 2];
		arbol->datos[2*pos + 2] = NULL;

		free(nodo->clave);
		free(nodo);
		
		return valor;
	}

	// Nodo con un solo hijo a izquierda
	if (nodo_con_un_solo_hijo_izquierdo(arbol, pos)) {

		nodo = arbol->datos[pos];
		arbol->datos[pos] = arbol->datos[2*pos + 1];
		arbol->datos[2*pos + 1] = NULL;

		free(nodo->clave);
		free(nodo);

		return valor;
	}

	borrar_recursivo(arbol, pos);

	return valor;
}

// Redimensiona el árbol cuando se termina el espacio.
static bool redimensionar_arbol(abb_t* arbol, size_t tam) {

	nodo_abb_t* *datos_nuevo = realloc(arbol->datos, tam*sizeof(nodo_abb_t*));
	
	if (!datos_nuevo) return false;

	arbol->datos = datos_nuevo;
	
	for (int i = arbol->tamanio; i < tam; i++) {

		arbol->datos[i] = NULL;
	}

	arbol->tamanio = tam;
	
	return true;
}

static bool in_order(abb_t* arbol, size_t pos_nodo, bool visitar(const char *, void *, void *), void *extra) {

	if (pos_nodo >= arbol->tamanio) return true;
	if (!arbol->datos[pos_nodo]) return true;

	nodo_abb_t* nodo = arbol->datos[pos_nodo];

	if (!in_order(arbol, 2*pos_nodo + 1, visitar, extra)) return false;
	if (!visitar(nodo->clave, nodo->dato, extra)) return false;
	if (!in_order(arbol, 2*pos_nodo + 2, visitar, extra)) return false;

	return true;
}

 /* ******************************************************************
 *                       PRIMITIVAS DEL ÁRBOL
 * *****************************************************************/

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {

	abb_t* arbol = malloc(sizeof(abb_t));
	
	if (!arbol) return NULL;

	arbol->datos = calloc(TAM_INICIAL*sizeof(nodo_abb_t*), sizeof(nodo_abb_t*));
	
	if (!arbol->datos) {
		free(arbol);
		return NULL;
	}

	arbol->tamanio = TAM_INICIAL;
	arbol->cantidad = 0;
	arbol->cmp = cmp;
	arbol->destruir_dato = destruir_dato;
	
	return arbol;
}

size_t abb_cantidad(const abb_t* arbol) {

	return arbol->cantidad;
}

bool abb_esta_vacio(const abb_t* arbol) {

	return abb_cantidad(arbol) == 0;
}

bool abb_pertenece(const abb_t* arbol, const char* clave) {

	if (abb_esta_vacio(arbol)) return false;

	size_t pos = busqueda(arbol, clave);

	if (pos < arbol->tamanio) {

		return arbol->datos[pos];
	}

	return false;
}

bool abb_guardar(abb_t* arbol, const char* clave, void* dato) {

	if (abb_esta_vacio(arbol)) {
		
		arbol->datos[0] = nodo_crear(clave, dato);
		arbol->cantidad = 1;
		
		return true;
	}

	size_t pos = busqueda(arbol, clave);

	if (abb_pertenece(arbol, clave)) {

		nodo_abb_t* nodo = arbol->datos[pos];

		void* valor = nodo->dato;

		if (arbol->destruir_dato) arbol->destruir_dato(valor);

		nodo->dato = dato;

		return true;
	}

	nodo_abb_t* nodo_nuevo = nodo_crear(clave, dato);

	if (pos >= arbol->tamanio) {
		
		if (redimensionar_arbol(arbol, FACTOR*arbol->tamanio)) {
			
			arbol->datos[pos] = nodo_nuevo;
			(arbol->cantidad)++;
			
			return true;
		}
		
		return false;

	} else {

		arbol->datos[pos] = nodo_nuevo;
		(arbol->cantidad)++;
		
		return true;
	}
}

void* abb_borrar(abb_t* arbol, const char* clave) {

	if (abb_esta_vacio(arbol)) return NULL;

	if (!abb_pertenece(arbol, clave)) return NULL;

	size_t pos = busqueda(arbol, clave);
	
	return borrar(arbol, pos);
}

void* abb_obtener(const abb_t* arbol, const char* clave) {

	if (abb_esta_vacio(arbol)) return NULL;

	if (!abb_pertenece(arbol, clave)) return NULL;

	size_t pos = busqueda(arbol, clave);

	nodo_abb_t* nodo = arbol->datos[pos];
	
	return nodo->dato;
}

void abb_destruir(abb_t* arbol) {

	abb_destruir_dato_t destruir_dato = arbol->destruir_dato;

	nodo_abb_t* nodo;

	for (int i = 0; i < arbol->tamanio; i++) {

		if (arbol->datos[i]) {
			
			nodo = arbol->datos[i];

			free(nodo->clave);
			
			if (destruir_dato) destruir_dato(nodo->dato);
			
			free(nodo);
		}
	}

	free(arbol->datos);
	free(arbol);
}

/* ******************************************************************
 *                      PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {

	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	
	if (!iter) return NULL;

	iter->arbol = arbol;

	iter->pila = pila_crear(NULL);

	if (abb_esta_vacio(iter->arbol)) return iter;

	pila_apilar(iter->pila, iter->arbol->datos[0]);

	int i = 1;
	nodo_abb_t* nodo_siguiente = iter->arbol->datos[i];
	
	while (nodo_siguiente != NULL) {
		
		pila_apilar(iter->pila, nodo_siguiente);
		i = 2*i + 1;
		nodo_siguiente = iter->arbol->datos[i];
	}

	return iter;
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {

	return pila_esta_vacia(iter->pila);
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter) {

	if (pila_esta_vacia(iter->pila)) return NULL;

	nodo_abb_t* actual = pila_ver_tope(iter->pila);
	
	if (!actual) return NULL;
	
	return actual->clave;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {

	if (abb_iter_in_al_final(iter)) return false;

	nodo_abb_t* actual = pila_desapilar(iter->pila);

	if (!actual) return false;

	size_t pos_actual = busqueda(iter->arbol, actual->clave);

	size_t pos_actual_der = 2*pos_actual + 2;

	if (pos_actual_der >= iter->arbol->tamanio) return true;

	nodo_abb_t* actual_derecho = iter->arbol->datos[pos_actual_der];

	if (!actual_derecho) return true;

	pila_apilar(iter->pila, actual_derecho);

	if ((2*pos_actual_der + 1) >= iter->arbol->tamanio) return true;

	nodo_abb_t* nodo_siguiente = iter->arbol->datos[2*pos_actual_der + 1];

	while (nodo_siguiente != NULL) {
		
		pila_apilar(iter->pila, nodo_siguiente);

		size_t pos_nodo_siguiente = busqueda(iter->arbol, nodo_siguiente->clave);

		nodo_siguiente = iter->arbol->datos[2*pos_nodo_siguiente + 1];
	}
	
	return true;
}

void abb_iter_in_destruir(abb_iter_t* iter) {
	
	pila_destruir(iter->pila);
	free(iter);
}

/* ******************************************************************
 *               PRIMITIVA ITERADOR INTERNO (in-order)
 * *****************************************************************/

void abb_in_order(abb_t* arbol, bool visitar(const char* clave, void* dato, void* extra), void* extra) {

	if (arbol == NULL || abb_esta_vacio(arbol) || visitar == NULL) return;

	in_order(arbol, 0, visitar, extra);
}
