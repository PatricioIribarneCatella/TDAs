#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pila.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef int (*abb_comparar_clave_t) (const char *, const char *);

typedef void (*abb_destruir_dato_t) (void *);

typedef struct nodo_abb {
	char* clave;
	void* dato;
	struct nodo_abb* izq;
	struct nodo_abb* der;
} nodo_abb_t;

typedef struct abb {
	nodo_abb_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cantidad;
} abb_t;

typedef struct abb_iter {
	pila_t* pila;
} abb_iter_t;

typedef struct padre_hijo {
	nodo_abb_t* padre;
	nodo_abb_t* hijo;
} padre_hijo_t;

/* ******************************************************************
 *                       FUNCIONES AUXILIARES
 * *****************************************************************/

// Crea un nuevo nodo con la clave y el dato que se le pasó.
static nodo_abb_t* nodo_crear(const char* clave, void* dato) {

	nodo_abb_t* nodo = malloc(sizeof(nodo_abb_t));

	if (!nodo) return NULL;
	
	nodo->clave = malloc(sizeof(char)*(strlen(clave) + 1));
	
	if (!nodo->clave) {
		free(nodo);
		return NULL;
	}
	
	strcpy(nodo->clave, clave);
	nodo->dato = dato;
	nodo->der = NULL;
	nodo->izq = NULL;
	
	return nodo;
}

// Busca una clave en el arbol. Devuelve una estructura con el nodo que la contiene y su padre.
static padre_hijo_t busqueda(const abb_t *arbol, const char *clave) {

	padre_hijo_t padre_hijo;
	padre_hijo.hijo = NULL;
	padre_hijo.padre = NULL;

	if (arbol->cantidad == 0) return padre_hijo;
	
	abb_comparar_clave_t cmp = arbol->cmp;
	padre_hijo.hijo = arbol->raiz;

	if (cmp(arbol->raiz->clave, clave) == 0) return padre_hijo;

	while (cmp(padre_hijo.hijo->clave, clave) != 0) {

		padre_hijo.padre = padre_hijo.hijo;
		
		if (cmp(padre_hijo.padre->clave, clave) > 0) {
			padre_hijo.hijo = padre_hijo.padre->izq;
		} else if (cmp(padre_hijo.padre->clave, clave) < 0) {
			padre_hijo.hijo = padre_hijo.padre->der;
		}
		
		if (padre_hijo.hijo == NULL) return padre_hijo;
	}

	return padre_hijo;
}

// Busca recursivamente una clave en el arbol. Si la encuentra devuelve el valor asociado y sino devuelve NULL.
static void* busqueda_recursiva(nodo_abb_t* nodo, abb_comparar_clave_t cmp, const char* clave) {

	if (cmp(nodo->clave, clave) == 0) return nodo->dato;

	if (cmp(nodo->clave, clave) > 0) {
		
		if (nodo->izq == NULL) return NULL;
		return busqueda_recursiva(nodo->izq, cmp, clave);
	}

	if (cmp(nodo->clave, clave) < 0) {
		
		if (nodo->der == NULL) return NULL;
		return busqueda_recursiva(nodo->der, cmp, clave);
	}

	return NULL;
}

// Busca recursivamente una clave en el arbol. Devuelve true o false dependiendo de si encuentra la clave o no.
static bool busqueda_recursiva_pertenece(nodo_abb_t* nodo, abb_comparar_clave_t cmp, const char* clave) {

	if (cmp(nodo->clave, clave) == 0) return true;

	if (cmp(nodo->clave, clave) > 0) {
		
		if (nodo->izq == NULL) return false;
		return busqueda_recursiva_pertenece(nodo->izq, cmp, clave);
	}

	if (cmp(nodo->clave, clave) < 0) {
		
		if (nodo->der == NULL) return false;
		return busqueda_recursiva_pertenece(nodo->der, cmp, clave);
	}

	return false;
}

// Se fija si el árbol está vacio o no.
static bool arbol_vacio(const abb_t* arbol) {

	return (arbol->cantidad == 0);
}

// Borra recursivamente el arbol en recorrido pos-order.
static void borrar_pos_order(abb_t* arbol, nodo_abb_t *nodo, void visitar(abb_t*, nodo_abb_t*)) {

	if (nodo == NULL) return;

	borrar_pos_order(arbol, nodo->izq, visitar);
	borrar_pos_order(arbol, nodo->der, visitar);
	
	visitar(arbol, nodo);
}

// Funcion de destrucción de contenidos de nodos.
static void destruir_nodo(abb_t* arbol, nodo_abb_t* nodo) {

	if (arbol->destruir_dato) arbol->destruir_dato(nodo->dato);
	
	free(nodo->clave);
	free(nodo);
}

// Busca el mínimo nodo del subarbol derecho.
static nodo_abb_t* buscar_minimo(abb_t* arbol, nodo_abb_t* actual, nodo_abb_t** padre) {

	nodo_abb_t* min = actual->der;
	*padre = actual;

	while (min->izq) {

		*padre = min;
		min = min->izq;
	}
	
	return min;
}

// Intercambia el contenido de dos nodos.
static void swap_nodos(nodo_abb_t* nodo1, nodo_abb_t* nodo2) {

	char* clave_aux = nodo1->clave;
	void* dato_aux = nodo1->dato;

	nodo1->clave = nodo2->clave;
	nodo1->dato = nodo2->dato;

	nodo2->clave = clave_aux;
	nodo2->dato = dato_aux;
}

// Borra recursivamente un nodo del arbol.
static void* borrar_dato_recursivo(abb_t* arbol, nodo_abb_t* actual, nodo_abb_t* padre, const char* clave) {

	if (!actual) {

		return NULL;

	} else if (arbol->cmp(clave, actual->clave) < 0) {
		
		return borrar_dato_recursivo(arbol, actual->izq, actual, clave);

	} else if (arbol->cmp(clave, actual->clave) > 0) {
		
		return borrar_dato_recursivo(arbol, actual->der, actual, clave);

	} else {
		// Caso 1: Borrar nodo sin hijos y es raiz del arbol.
		if ((!padre) && (!actual->der) && (!actual->izq)) {

			arbol->raiz = NULL;
			void* dato = actual->dato;
			free(actual->clave);
			free(actual);
			arbol->cantidad--;
			return dato;
		}

		// Caso 2: Borrar nodo con solo hijo derecho y es raiz del arbol
		if ((!padre) && (!actual->izq) && (actual->der)) {

			arbol->raiz = actual->der;
			void* dato = actual->dato;
			free(actual->clave);
			free(actual);
			arbol->cantidad--;
			return dato;
		}

		// Caso 3: Borrar nodo con solo hijo izquierdo y es raiz del arbol
		if ((!padre) && (!actual->der) && (actual->izq)) {

			arbol->raiz = actual->izq;
			void* dato = actual->dato;
			free(actual->clave);
			free(actual);
			arbol->cantidad--;
			return dato;
		}

		// Caso 4: Borrar nodo sin hijos y no es raiz del arbol.
		if ((!actual->der) && (!actual->izq)) {

			if ((padre->der) && (padre->der == actual)) padre->der = NULL;
			else if ((padre->izq) && (padre->izq == actual)) padre->izq = NULL;
			void* dato = actual->dato;
			free(actual->clave);
			free(actual);
			arbol->cantidad--;
			return dato;
		}

		// Caso 5: Borrar nodo con solo hijo derecho
		if ((!actual->izq) && (actual->der)) {

			if ((padre->der) && (padre->der == actual)) padre->der = actual->der;
			else if ((padre->izq) && (padre->izq == actual)) padre->izq = actual->der;
			void* dato = actual->dato;
			free(actual->clave);
			free(actual);
			arbol->cantidad--;
			return dato;
		}

		// Caso 6: Borrar nodo con solo hijo izquierdo
		if ((!actual->der) && (actual->izq)) {

			if ((padre->der) && (padre->der == actual)) padre->der = actual->izq;
			else if ((padre->izq) && (padre->izq == actual)) padre->izq = actual->izq;
			void* dato = actual->dato;
			free(actual->clave);
			free(actual);
			arbol->cantidad--;
			return dato;
		}

		// Caso 7: Borrar nodo con dos hijos
		bool flag = false;
		nodo_abb_t* padre_del_min = actual;

		if ((actual->der) && (actual->izq)) {

			nodo_abb_t* min = buscar_minimo(arbol, actual, &padre_del_min);
			
			if (actual != padre_del_min) flag = true;
			
			swap_nodos(min, actual);
			nodo_abb_t* borrado = min;
			
			if (flag) padre_del_min->izq = min->der;
			else padre_del_min->der = min->der;

			void* dato = borrado->dato;
			free(borrado->clave);
			free(borrado);
			arbol->cantidad--;
			return dato;
		}

	}

	return NULL;
}

// Realiza recorrido in-order del arbol.
static bool in_order(nodo_abb_t *nodo, bool visitar(const char *, void *, void *), void *extra) {

	if (nodo == NULL) return true;
	
	if (!in_order(nodo->izq, visitar, extra)) return false;
	if (!visitar(nodo->clave, nodo->dato, extra)) return false;
	if (!in_order(nodo->der, visitar, extra)) return false;

	return true;
}

/* ******************************************************************
 *                      PRIMITIVAS DEL ÁRBOL
 * *****************************************************************/

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {

	abb_t* arbol = malloc(sizeof(abb_t));
	
	if (!arbol) return NULL;
	
	arbol->raiz = NULL;
	arbol->cantidad = 0;
	arbol->cmp = cmp;
	arbol->destruir_dato = destruir_dato;
	
	return arbol;
}

bool abb_guardar(abb_t* arbol, const char* clave, void* dato) {

	padre_hijo_t padre_hijo = busqueda(arbol, clave);

	if (padre_hijo.hijo == NULL) {
		
		nodo_abb_t *nodo_nuevo = nodo_crear(clave, dato);
		
		if (arbol->cantidad == 0) {
			arbol->raiz = nodo_nuevo;
			arbol->cantidad = 1;
			return true;
		}

		abb_comparar_clave_t cmp = arbol->cmp;
		
		if (cmp(padre_hijo.padre->clave, clave) > 0) {

			padre_hijo.padre->izq = nodo_nuevo;

		} else {
			
			padre_hijo.padre->der = nodo_nuevo;
		}
		
		(arbol->cantidad)++;
		
		return true;

	} else {
		
		abb_destruir_dato_t destruir_dato = arbol->destruir_dato;
		void* valor = padre_hijo.hijo->dato;
		
		if (destruir_dato) {
			destruir_dato(valor);
		}
		
		padre_hijo.hijo->dato = dato;
		
		return true;
	}
}

size_t abb_cantidad(abb_t *arbol) {

	return arbol->cantidad;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {

	if (arbol_vacio(arbol)) return false;
	return busqueda_recursiva_pertenece(arbol->raiz, arbol->cmp, clave);
}

void* abb_obtener(const abb_t *arbol, const char *clave) {

	if (arbol_vacio(arbol)) return NULL;
	return busqueda_recursiva(arbol->raiz, arbol->cmp, clave);
}

void* abb_borrar(abb_t *arbol, const char *clave) {

	if (!abb_pertenece(arbol, clave)) return NULL;
	return borrar_dato_recursivo(arbol, arbol->raiz, NULL, clave);
}

void abb_destruir(abb_t *arbol) {

	borrar_pos_order(arbol, arbol->raiz, destruir_nodo);
	free(arbol);
}

/* ******************************************************************
 *                     PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {

	abb_iter_t *iter = malloc (sizeof(abb_iter_t));
	
	if (!iter) return NULL;

	iter->pila = pila_crear(NULL);

	if (arbol_vacio(arbol)) return iter;

	pila_apilar(iter->pila, arbol->raiz);
	nodo_abb_t *nodo_siguiente = arbol->raiz->izq;
	
	while (nodo_siguiente != NULL) {
		
		pila_apilar(iter->pila, nodo_siguiente);
		nodo_siguiente = nodo_siguiente->izq;
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

	if (!actual->der) return true;

	pila_apilar(iter->pila, actual->der);
	nodo_abb_t *nodo_siguiente = actual->der->izq;
	
	while (nodo_siguiente != NULL) {
		
		pila_apilar(iter->pila, nodo_siguiente);
		nodo_siguiente = nodo_siguiente->izq;
	}
	
	return true;
}

void abb_iter_in_destruir(abb_iter_t* iter) {

	pila_destruir(iter->pila);
	free(iter);
}

/* ******************************************************************
 *             PRIMITIVA DEL ITERADOR INTERNO (in-order)
 * *****************************************************************/

void abb_in_order(abb_t *arbol, bool visitar(const char *clave, void *dato, void *extra), void *extra) {

	if (arbol == NULL || arbol->raiz == NULL || visitar == NULL) return;
	in_order(arbol->raiz, visitar, extra);
}
