#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef void (*grafo_destruir_dato_t) (void *);

typedef struct grafo grafo_t;

/* ******************************************************************
 *                              GRAFO
 * *****************************************************************/

// Crea un nuevo grafo vacio.
grafo_t* grafo_crear(grafo_destruir_dato_t destruir_dato);

// Inserta una nueva arista en el grafo, recibiendo como parametros los dos nombres de los vertices a unir.
// Pre: el grafo fue creado.
// Post: una arista fue creada.
bool grafo_insertar_arista(grafo_t* grafo, char* vertice1, char* vertice2);

// Inserta un nuevo vertice.
// Pre: el grafo fue creado.
// Post: un nuevo vertice fue creado.
bool grafo_insertar_vertice(grafo_t* grafo, char* clave, void* dato);

// Borra una arista.
// Pre: el grafo fue creado y la arista existe.
// Post: se borro la arista que unia los dos vertices que se pasan por parametro.
bool grafo_borrar_arista(grafo_t* grafo, char* vertice1, char* vertice2);

// Borra un vertice.
// Pre: el grafo fue creado y el vertice existe.
// Post: se borro el vertice deseado.
void* grafo_borrar_vertice(grafo_t* grafo, char* vertice);

// Devuelve la cantidad de vertices que posee el grafo.
// Pre: el grafo fue creado.
// Post: devuelve la cantidad de vertices.
size_t grafo_cantidad_vertices(const grafo_t* grafo);

// Devuelve la cantidad de aristas que posee el grafo.
// Pre: el grafo fue creado.
// Post: devuelve la cantidad de aristas.
size_t grafo_cantidad_aristas(const grafo_t* grafo);

// Devuelve la cantidad de adyacentes a un vertice.
// Pre: el grafo fue creado y el vertice existe.
// Post: devuelve la cantidad de adyacentes a un vertice.
size_t grafo_cant_adyacentes(grafo_t* grafo, char* vertice);

// Verifica si el grafo esta vacio.
// Pre: el grafo fue creado.
// Post: devuelve verdero en caso de estar vacio o falso en caso contrario.
bool grafo_esta_vacio(const grafo_t* grafo);

// Verifica si un vertice pertenece al grafo.
// Pre: el grafo fue creado.
// Post: devuelve verdero en caso de que el vertice perteneza al grafo o falso en caso contrario.
bool grafo_pertence(const grafo_t* grafo, const char* vertice);

// Verifica si un vertice es adyacente a otro.
// Pre: el grafo fue creado y los dos vertices a considerar existen dentro del grafo.
// Post: devuelve verdadero si eran adyacentes o falso en caso contrario.
bool grafo_adyacente_pertence(const grafo_t* grafo, const char* vertice, const char* adyacente);

// Devuelve una lista simplemente enlazada con los nombres de los adyacentes a un vertices.
// Pre: el grafo fue creado y el vertice existe dentro del grafo.
// Post: devuelve la lista con los nombres de los adyacentes.
lista_t* grafo_adyacentes(grafo_t* grafo, const char* vertice);

// Devuelve una lista simplemente enlazada con los vertices del grafo.
// Pre: el grafo fue creado.
// Post: devuelve la lista si el grafo no esta vacio o NULL en caso contrario.
lista_t* grafo_vertices(const grafo_t* grafo);

// Destruye el grafo.
// Pre: el grafo fue creado.
// Post: se destruyó el grafo y todo su contenido.
void grafo_destruir(grafo_t* grafo);

#endif // GRAFO_H
