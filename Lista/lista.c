#include <stdlib.h>
#include <stdbool.h>

/* ******************************************************************
 *              DEFINICION DE LOS TIPOS DE DATOS LISTA
 * *****************************************************************/

typedef struct nodo_lista {
	void* *datos;
	struct nodo_lista *siguiente;
} nodo_lista_t;

typedef struct lista {
	nodo_lista_t *primero;
	nodo_lista_t *ultimo;
	size_t largo;
} lista_t;

typedef struct lista_iter {
	nodo_lista_t *actual;
	nodo_lista_t *anterior;
} lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

lista_t* lista_crear() {

	lista_t *lista = malloc(sizeof(lista_t));
	
	if (!lista) return NULL;

	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->largo = 0;
	
	return lista;
}

bool lista_esta_vacia(const lista_t *lista) {

	return (lista->primero == NULL && lista->ultimo == NULL);
}

bool lista_insertar_primero(lista_t *lista, void *dato) {

	nodo_lista_t *nodo_lista = malloc(sizeof(nodo_lista_t));
	
	if (!nodo_lista) return false;

	nodo_lista->datos = dato;
	
	if (lista_esta_vacia(lista)) {
		
		nodo_lista->siguiente = NULL;
		lista->primero = nodo_lista;
		lista->ultimo = nodo_lista;
		lista->largo = 1;
		
		return true;
	}
	
	nodo_lista->siguiente = lista->primero;
	lista->primero = nodo_lista;
	(lista->largo)++;
	
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato) {

	nodo_lista_t *nodo_lista = malloc(sizeof(nodo_lista_t));
	
	if (!nodo_lista) return false;

	nodo_lista->datos = dato;
	nodo_lista->siguiente = NULL;
	
	if (lista_esta_vacia(lista)) {
		
		lista->primero = nodo_lista;
		lista->ultimo = nodo_lista;
		lista->largo = 1;
		
		return true;
	}
	
	lista->ultimo->siguiente = nodo_lista;
	lista->ultimo = nodo_lista;
	(lista->largo)++;
	
	return true;
}

void *lista_borrar_primero(lista_t *lista) {

	if (lista_esta_vacia(lista)) return NULL;

	void* valor = lista->primero->datos;
	
	nodo_lista_t* nodo_siguiente = lista->primero->siguiente;
	nodo_lista_t* nodo = lista->primero;
	
	free (nodo);
	
	lista->primero = nodo_siguiente;
	(lista->largo)--;
	
	if (lista->primero == NULL) {
		lista->ultimo = NULL;
	}
	
	return valor;
}

void *lista_ver_primero(const lista_t *lista) {

	if (lista_esta_vacia(lista)) return NULL;
	
	return lista->primero->datos;
}

size_t lista_largo(const lista_t *lista) {

	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)) {

	void* dato;

	while (!lista_esta_vacia(lista)) {
		
		dato = lista_borrar_primero(lista);
		
		if (destruir_dato) {
			destruir_dato(dato);
		}
	}

	free (lista);
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

lista_iter_t *lista_iter_crear(const lista_t *lista) {

	lista_iter_t *iter = malloc(sizeof(lista_iter_t));
	
	if (!iter) return NULL;
	
	iter->actual = lista->primero;
	iter->anterior = NULL;
	
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter) {

	if (!iter->actual) return false;

	iter->anterior = iter->actual;
	iter->actual = iter->actual->siguiente;
	
	return true;
}

bool lista_iter_al_final(const lista_iter_t *iter) {

	return (iter->actual == NULL);
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {

	if (lista_iter_al_final(iter)) return NULL;
	
	return iter->actual->datos;
}

void lista_iter_destruir(lista_iter_t *iter) {

	free (iter);
}

/* ******************************************************************
 *                PRIMITIVAS DEL ITERADOR EN LA LISTA
 * *****************************************************************/

bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato) {

	if (iter->anterior == NULL) {

		lista_insertar_primero (lista, dato);
		iter->actual = lista->primero;

	} else if (iter->actual == NULL) {
		
		lista_insertar_ultimo(lista, dato);
		iter->actual = lista->ultimo;

	}else {
		
		nodo_lista_t *nodo_lista = malloc(sizeof(nodo_lista_t));
		
		if (!nodo_lista) return false;
		
		nodo_lista->siguiente = iter->actual;
		nodo_lista->datos = dato;
		iter->actual = nodo_lista;
		iter->anterior->siguiente = nodo_lista;
		(lista->largo)++;
	}

	return true;
}

void *lista_borrar(lista_t *lista, lista_iter_t *iter) {

	if (lista_esta_vacia(lista)) return  NULL;
	
	if (lista_iter_al_final(iter)) return NULL;

	void* valor = iter->actual->datos;
	nodo_lista_t* nodo_siguiente = iter->actual->siguiente;

	if (iter->anterior == NULL) {

		nodo_lista_t *nodo = lista->primero;
		free(nodo);
		lista->primero = nodo_siguiente;

	} else {
		
		nodo_lista_t* nodo = iter->anterior->siguiente;
		free (nodo);
		iter->anterior->siguiente = nodo_siguiente;
	}

	iter->actual = nodo_siguiente;
	lista->largo = lista->largo -1;
	
	if (lista_iter_al_final(iter)) {
		lista->ultimo = iter->anterior;
	}
	
	return valor;
}

/* ******************************************************************
 *                 PRIMITIVA DEL ITERADOR INTERNO
 * *****************************************************************/

void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra) {

	if (!lista_esta_vacia(lista)) {

		void *dato = lista->primero->datos;
		
		nodo_lista_t *nsiguiente = lista->primero->siguiente;
		
		while (visitar(dato, extra) && nsiguiente != NULL) {
			
			dato = nsiguiente->datos;
			nsiguiente = nsiguiente->siguiente;
		}
	}
}
