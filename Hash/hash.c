#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lista.h"

#define TAM_INICIAL 101
#define MAX_FACTOR_DE_CARGA 1.5
#define MIN_FACTOR_DE_CARGA 0.25
#define FACTOR_MULTIPLICACION 3

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef void (*hash_destruir_dato_t) (void *);

typedef size_t (*f_hash_t) (const char* clave, size_t tam);

typedef struct hash {
	void* *datos;
	size_t cantidad_elementos;
	size_t tamanio;
	hash_destruir_dato_t destruir_dato;
	f_hash_t fhash;
} hash_t;

typedef struct clave_valor {
	char *clave;
	void *valor;
} clave_valor_t;

typedef struct hash_iter {
	size_t posicion_actual;
	lista_iter_t *lista_iter;
	const hash_t* hash;
	bool al_final;
} hash_iter_t;

/* ******************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

// Crea un nuevo nodo con la clave y su correspondiente valor asociado.
static clave_valor_t* hash_crear_nodo(const char* clave, void* dato) {

	clave_valor_t* nodo = malloc(sizeof(clave_valor_t));

	if (!nodo) return NULL;

	nodo->clave = malloc(sizeof(char)*(strlen(clave)+1));

	if (!nodo->clave) {
		free(nodo);
		return NULL;
	}

	strcpy(nodo->clave, clave);
	nodo->valor = dato;

	return nodo;
}

// Destruye un nodo devolviendo el valor almacenado.
static void* hash_destuir_nodo(clave_valor_t* nodo) {

	void* dato = nodo->valor;
	free(nodo->clave);
	free(nodo);

	return dato;
}

// Verifica si el hash está vacio
static bool hash_esta_vacio(const hash_t* hash) {

	return hash->cantidad_elementos == 0;
}

// Busca una clave en el Hash.
static bool hash_buscar(const hash_t* hash, const char* clave, lista_iter_t* iter) {

	size_t pos = hash->fhash(clave,hash->tamanio);

	if (hash_esta_vacio(hash) || lista_esta_vacia(hash->datos[pos])) return false;

	clave_valor_t *clave_valor_aux;
	char *clave_aux;

	while (!lista_iter_al_final(iter)) {

		clave_valor_aux = lista_iter_ver_actual(iter);
		clave_aux = clave_valor_aux->clave;

		if (strcmp(clave_aux,clave) == 0) return true;

		lista_iter_avanzar(iter);
	}

	return false;
}

// Devuelve la posición en el hash donde está el próxima lista guardada.
static size_t hash_proximo_elemento(const hash_t* hash,size_t posicion_inicial) {

	size_t i = posicion_inicial + 1;

	while((i < hash->tamanio) && lista_esta_vacia(hash->datos[i]))
		i++;

	return i;
}

// Devuelve true o false dependiendo de si el factor de carga se supero o no.
static bool factor_hash_superado(hash_t* hash) {

	float cantidad_elementos = hash->cantidad_elementos;
	float tamanio = hash->tamanio;

	return (cantidad_elementos/tamanio) >= MAX_FACTOR_DE_CARGA;
}

// Redimensiona el Hash cuando el factor de carga se supera.
static bool hash_redimensionar(hash_t* hash) {

	int nuevo_tamanio = hash->tamanio * FACTOR_MULTIPLICACION;

	void* *datos_viejos = hash->datos;
	hash->datos = malloc(sizeof(void*)*nuevo_tamanio);

	if(!hash->datos) return false;

	for (int i = 0; i < nuevo_tamanio; i++) {
		hash->datos[i] = lista_crear();
	}

	int posicion_nueva;
	clave_valor_t* aux_nodo;

	for (int i = 0; i < hash->tamanio; i++) {

		while (!lista_esta_vacia(datos_viejos[i])) {

			aux_nodo = lista_borrar_primero(datos_viejos[i]);
			posicion_nueva = hash->fhash(aux_nodo->clave,nuevo_tamanio);
			lista_insertar_primero(hash->datos[posicion_nueva],aux_nodo);
		}

		lista_destruir(datos_viejos[i], NULL);
	}

	free(datos_viejos);
	hash->tamanio = nuevo_tamanio;
	
	return true;
}

/* ******************************************************************
 *                    PRIMITIVAS DEL HASH
 * ******************************************************************/

hash_t* hash_crear(hash_destruir_dato_t destruir_dato, f_hash_t fhash) {

	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;


	hash->datos = malloc(sizeof(void*)*TAM_INICIAL);

	if (!hash->datos) {
		free(hash);
		return NULL;
	}

	for (int i = 0; i < TAM_INICIAL; i++) {
		hash->datos[i] = lista_crear();
	}

	hash->cantidad_elementos = 0;
	hash->tamanio = TAM_INICIAL;
	hash->destruir_dato = destruir_dato;
	hash->fhash = fhash;

	return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {

	if (factor_hash_superado(hash)) {

		if (!hash_redimensionar(hash)) return false;
	}

	bool salida = true;
	size_t posicion = hash->fhash(clave, hash->tamanio);

	lista_t* lista = hash->datos[posicion];
	lista_iter_t* iter = lista_iter_crear(lista);

	if (!hash_buscar(hash, clave, iter)) {

		clave_valor_t* nuevo_nodo = hash_crear_nodo(clave, dato);

		if (nuevo_nodo == NULL)
			
			salida = false;

		else {
			
			lista_insertar_primero(lista, nuevo_nodo);
			(hash->cantidad_elementos)++;
		}

	} else {

		clave_valor_t* aux = lista_iter_ver_actual(iter);

		if (hash->destruir_dato) hash->destruir_dato(aux->valor);
		
		aux->valor = dato;
	}

	lista_iter_destruir(iter);

	return salida;
}

void* hash_borrar_dato(hash_t *hash, const char *clave) {

	void* salida = NULL;

	size_t posicion = hash->fhash(clave, hash->tamanio);

	lista_t* lista = hash->datos[posicion];
	lista_iter_t* iter = lista_iter_crear(lista);

	if (hash_buscar(hash, clave, iter)) {

		clave_valor_t* aux = lista_borrar(lista, iter);
		salida = aux->valor;
		free(aux->clave);
		free(aux);
	}

	if (salida != NULL) (hash->cantidad_elementos)--;

	lista_iter_destruir(iter);

	return salida;
}

bool hash_borrar(hash_t *hash, const char *clave) {

	void* salida = NULL;

	size_t posicion = hash->fhash(clave, hash->tamanio);

	lista_t* lista = hash->datos[posicion];
	lista_iter_t* iter = lista_iter_crear(lista);

	if (hash_buscar(hash, clave, iter)) {

		clave_valor_t* aux = lista_borrar(lista, iter);
		salida = aux->valor;
		free(aux->clave);
		free(aux);
	}

	if (salida != NULL) (hash->cantidad_elementos)--;

	lista_iter_destruir(iter);

	return (salida != NULL);
}

void* hash_obtener(const hash_t *hash, const char *clave) {

	size_t posicion = hash->fhash(clave, hash->tamanio);

	lista_t* lista = hash->datos[posicion];
	lista_iter_t* iter = lista_iter_crear(lista);

	if (hash_buscar(hash, clave, iter)) {
		
		clave_valor_t* aux = lista_iter_ver_actual(iter);
		lista_iter_destruir(iter);
		return aux->valor;
	}

	lista_iter_destruir(iter);

	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {

	size_t posicion = hash->fhash(clave, hash->tamanio);

	lista_t* lista = hash->datos[posicion];
	lista_iter_t* iter = lista_iter_crear(lista);

	bool salida = hash_buscar(hash, clave, iter);
	lista_iter_destruir(iter);

	return salida;
}

size_t hash_cantidad(const hash_t *hash) {

	return hash->cantidad_elementos;
}

lista_t* hash_claves(const hash_t* hash) {

	lista_t* lista = lista_crear();

	int i = 0;
	
	while (i < hash->tamanio) {
		
		lista_t* lista_aux = hash->datos[i];
		
		if (lista_esta_vacia(lista_aux)) {
			
			i++;

		} else {

			lista_iter_t* iter = lista_iter_crear(lista_aux);
			
			while (!lista_iter_al_final(iter)) {

				clave_valor_t* clave_valor = lista_iter_ver_actual(iter);
				lista_insertar_primero(lista, clave_valor->clave);
				lista_iter_avanzar(iter);
			}
			
			i++;
			
			lista_iter_destruir(iter);
		}

	}

	return lista;
}

void hash_destruir(hash_t *hash) {

	void* aux_valor;

	hash_destruir_dato_t destruir_dato = hash->destruir_dato;

	for (int i = 0; i < hash->tamanio; i++) {

		while (!lista_esta_vacia(hash->datos[i])) {

			aux_valor = hash_destuir_nodo(lista_borrar_primero(hash->datos[i]));

			if (destruir_dato) destruir_dato(aux_valor);
		}

		lista_destruir(hash->datos[i], NULL);
	}

	free(hash->datos);
	free(hash);
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash) {

	size_t proximo_elemento;
	hash_iter_t* iter_nuevo = malloc(sizeof(hash_iter_t));

	if (!iter_nuevo) return NULL;

	iter_nuevo->posicion_actual = 0;
	iter_nuevo->al_final = true;
	proximo_elemento = hash_proximo_elemento(hash,-1);

	if (proximo_elemento != hash->tamanio) {

		iter_nuevo->posicion_actual = proximo_elemento;
		iter_nuevo->al_final = false;
	}

	iter_nuevo->hash = hash;
	iter_nuevo->lista_iter = lista_iter_crear(hash->datos[iter_nuevo->posicion_actual]);

	return iter_nuevo;
}

bool hash_iter_avanzar(hash_iter_t *iter) {

	if (iter->al_final) return false;

	size_t proximo_elemento = 0;

	lista_iter_avanzar(iter->lista_iter);

	if (lista_iter_al_final(iter->lista_iter)) {

		proximo_elemento = hash_proximo_elemento(iter->hash,iter->posicion_actual);

		if (proximo_elemento != iter->hash->tamanio) {

			lista_iter_destruir(iter->lista_iter);
			iter->posicion_actual = proximo_elemento;
			iter->lista_iter = lista_iter_crear(iter->hash->datos[iter->posicion_actual]);

		} else {

			iter->al_final = true;
			return false;
		}
	}

	return true;
}

const char* hash_iter_ver_actual(const hash_iter_t *iter) {

	if (iter->al_final)	return NULL;

	char* devolucion = NULL;

	clave_valor_t* aux = lista_iter_ver_actual(iter->lista_iter);

	if (aux != NULL) devolucion = aux->clave;

	return devolucion;
}

bool hash_iter_al_final(const hash_iter_t *iter) {

	return iter->al_final;
}

void hash_iter_destruir(hash_iter_t* iter) {

	lista_iter_destruir(iter->lista_iter);
	free(iter);
}
