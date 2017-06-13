#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lista.h"
#include "hash.h"

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef void (*grafo_destruir_dato_t) (void *);

typedef struct grafo {
	hash_t* vertices;
	size_t cantidad_vertices;
	size_t cantidad_aristas;
	grafo_destruir_dato_t destruir_dato;
} grafo_t;

typedef struct vertice {
	hash_t* adyacentes;
	char* clave;
	void* dato;
	size_t cant_adyacentes;
} vertice_t;

/* ******************************************************************
 *                       FUNCIONES AUXILIARES
 * *****************************************************************/

static vertice_t* crear_vertice(const char* clave, void* dato) {

	vertice_t* vertice = malloc(sizeof(vertice_t));
	
	if (!vertice) return NULL;

	vertice->clave = malloc(sizeof(char)*(strlen(clave) + 1));
	
	if (!vertice->clave) {
		free(vertice);
		return NULL;
	}

	strcpy(vertice->clave, clave);
	vertice->dato = dato;
	vertice->adyacentes = hash_crear_default(NULL);
	vertice->cant_adyacentes = 0;

	return vertice;
}

/* ******************************************************************
 *                      PRIMITIVAS DEL GRAFO
 * *****************************************************************/

grafo_t* grafo_crear(grafo_destruir_dato_t destruir_dato) {

	grafo_t* grafo = malloc(sizeof(grafo_t));
	
	if (!grafo) return NULL;

	grafo->vertices = hash_crear_default(NULL);
	grafo->destruir_dato = destruir_dato;
	grafo->cantidad_aristas = 0;
	grafo->cantidad_vertices = 0;
	
	return grafo;
}

bool grafo_insertar_arista(grafo_t* grafo, char* vertice1, char* vertice2) {

	if (!hash_pertenece(grafo->vertices, vertice1) &&
		!hash_pertenece(grafo->vertices, vertice2)) return false;

	vertice_t* vertice_aux1 = hash_obtener(grafo->vertices, vertice1);
	
	hash_guardar(vertice_aux1->adyacentes, vertice2, vertice2);
	(vertice_aux1->cant_adyacentes)++;

	vertice_t* vertice_aux2 = hash_obtener(grafo->vertices, vertice2);
	hash_guardar(vertice_aux2->adyacentes, vertice1, vertice1);
	
	(vertice_aux2->cant_adyacentes)++;
	(grafo->cantidad_aristas)++;
	
	return true;
}

bool grafo_insertar_vertice(grafo_t* grafo, char* clave, void* dato) {

	if (hash_pertenece(grafo->vertices, clave)) return false;

	vertice_t* vertice_nuevo = crear_vertice(clave, dato);
	
	if (!vertice_nuevo) return false;

	hash_guardar(grafo->vertices, clave, vertice_nuevo);
	(grafo->cantidad_vertices)++;
	
	return true;
}

bool grafo_borrar_arista(grafo_t* grafo, char* vertice1, char* vertice2) {

	if (!hash_pertenece(grafo->vertices, vertice1) &&
		!hash_pertenece(grafo->vertices, vertice2)) return false;

	vertice_t* vertice_aux1 = hash_obtener(grafo->vertices, vertice1);
	
	hash_borrar(vertice_aux1->adyacentes, vertice2);
	(vertice_aux1->cant_adyacentes)--;

	vertice_t* vertice_aux2 = hash_obtener(grafo->vertices, vertice2);
	hash_borrar(vertice_aux2->adyacentes, vertice1);
	
	(vertice_aux2->cant_adyacentes)--;
	(grafo->cantidad_aristas)--;
	
	return true;
}

void* grafo_borrar_vertice(grafo_t* grafo, char* vertice) {

	if (!hash_pertenece(grafo->vertices, vertice)) return NULL;

	vertice_t* vertice_aux = hash_borrar_dato(grafo->vertices, vertice);
	void* valor = vertice_aux->dato;

	lista_t* lista_claves = hash_claves(vertice_aux->adyacentes);
	lista_iter_t* iter = lista_iter_crear(lista_claves);
	
	while (!lista_iter_al_final(iter)) {
		
		vertice_t* vertice_aux2 = hash_obtener(grafo->vertices, lista_iter_ver_actual(iter));
		hash_borrar(vertice_aux2->adyacentes, vertice);
		lista_iter_avanzar(iter);
	}

	lista_iter_destruir(iter);
	lista_destruir(lista_claves, NULL);

	grafo->cantidad_aristas = grafo->cantidad_aristas - vertice_aux->cant_adyacentes;
	free(vertice_aux->clave);
	hash_destruir(vertice_aux->adyacentes);
	free(vertice_aux);

	(grafo->cantidad_vertices)--;
	
	return valor;
}

size_t grafo_cantidad_vertices(const grafo_t* grafo) {

	return grafo->cantidad_vertices;
}

size_t grafo_cantidad_aristas(const grafo_t* grafo) {

	return grafo->cantidad_aristas;
}

size_t grafo_cant_adyacentes(grafo_t* grafo, char* vertice) {

	vertice_t* vertice_aux = hash_obtener(grafo->vertices, vertice);
	
	return vertice_aux->cant_adyacentes;
}

bool grafo_esta_vacio(const grafo_t* grafo) {

	return (grafo_cantidad_vertices(grafo) == 0);
}

bool grafo_pertence(const grafo_t* grafo, const char* vertice) {

	return hash_pertenece(grafo->vertices, vertice);
}

bool grafo_adyacente_pertence(const grafo_t* grafo, const char* vertice, const char* adyacente) {

	vertice_t* vertice_aux = hash_obtener(grafo->vertices, vertice);
	
	if (!vertice_aux) return false;
	
	return hash_pertenece(vertice_aux->adyacentes, adyacente);
}

lista_t* grafo_adyacentes(grafo_t* grafo, const char* vertice) {

	if (!hash_pertenece(grafo->vertices, vertice)) return NULL;
	
	vertice_t* vertice_aux = hash_obtener(grafo->vertices, vertice);
	lista_t* lista_claves = hash_claves(vertice_aux->adyacentes);
	
	return lista_claves;
}

lista_t* grafo_vertices(const grafo_t* grafo) {

	if (!grafo_esta_vacio(grafo))
		return hash_claves(grafo->vertices);
	
	return NULL;
}

void grafo_destruir(grafo_t* grafo) {

	if (!grafo_esta_vacio(grafo)) {

		lista_t* lista_claves = hash_claves(grafo->vertices);
		lista_iter_t* iter = lista_iter_crear(lista_claves);
		grafo_destruir_dato_t destruir_dato = grafo->destruir_dato;

		while (!lista_iter_al_final(iter)) {

			vertice_t* vertice_aux = hash_borrar_dato(grafo->vertices, lista_iter_ver_actual(iter));
			free(vertice_aux->clave);
			
			if (destruir_dato) {
				destruir_dato(vertice_aux->dato);
			}
			
			hash_destruir(vertice_aux->adyacentes);
			free(vertice_aux);
			lista_iter_avanzar(iter);
		}

		lista_iter_destruir(iter);
		lista_destruir(lista_claves, NULL);
	}
	
	hash_destruir(grafo->vertices);
	free(grafo);
}
