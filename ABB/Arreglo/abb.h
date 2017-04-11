#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

// Tipo utilizado para el árbol binario.
typedef struct abb abb_t;

// Tipo utilizado para el iterador del árbol.
typedef struct abb_iter abb_iter_t;

// Función utilizada para comparar claves en caso de que la comparacin no sea la estadar. Se pasa como puntero a función por parámetro.
typedef int (*abb_comparar_clave_t) (const char *, const char *);

// Función utilizada para destruir cada dato del árbol ya que como son punteros a void no se conocen sus caracteríticas del tipo de dato que es.
typedef void (*abb_destruir_dato_t) (void *);

/* ******************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

// Crea un árbol.
// Post: devuelve un nuevo árbol vacío.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Guarda un nuevo elemnto en el árbol teniendo en cuenta que es un árbol binario de búsqueda con lo cual,
// del lado izquierdo los valores son menores que del lado derecho de la raiz.
// Pre: el árbol fue creado.
// Post: devuelve verdadero si se pudo guardar el elemento o falso en caso contrario.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Borra un elemento del árbol asociado a la clave que se la pasa como parámetro.
// Pre: el árbol fue creado.
// Post: devuelve el elemento asociado a la clave y lo borra del árbol, o devuelve NULL en caso de que la clave no existiese.
void *abb_borrar(abb_t *arbol, const char *clave);

// Devuelve el valor asociado a una clave que se pasa como parámetro.
// Pre: el árbol fue creado.
// Post: devueve el valor asociado a una clave sin borrar el elemento del árbol si la clave existía. De lo contrario devuelve NULL.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Se fija si una clave existe o no en el árbol binario de búsqueda.
// Pre: el árbol fue creado.
// Post: devuelve verdadero o falso en caso de que la clave existiera o no en el árbol.
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de elementos guardados en el árbol.
// Pre: el árbol fue creado.
// Post: devuelve la cantidad de elementos que hay en el árbol. Es cero si está vacío.
size_t abb_cantidad(abb_t *arbol);

// Destruye el árbol con todos sus elementos dentro.
// Pre: el árbol fue creado.
// Post: el árbol y todos sus elementos contenidos en él fueron destruidos.
void abb_destruir(abb_t *arbol);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

// Crea un nuevo iterador para recorrer el árbol en el sentido "in-order". Es decir la raíz siempre queda en el medio de dos subárboles.
// Pre: el árbol fue creado.
// Post: devuelve un nuevo puntero que se encuentra en el hijo mas chico del lado izquierdo.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza el iterador una posición.
// Pre: el iterador fue creado.
// Post: avanzó una nueva posición en el árbol.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave a la cual está apuntado el iterador en ese momento.
// Pre: el iterador fue creado.
// Post: devuelve una clave.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Se fija si el iterador se encuentra al final del árbol, es decir, si ya pasó por todos lo elementos del árbol.
// Pre: el iterador fue creado.
// Post: devuelve verdadero o falso en caso de haber recorrido todo el árbol o no.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador.
// Pre: el iterador fue creado.
// Post: destruyee el iterador.
void abb_iter_in_destruir(abb_iter_t* iter);

/* ******************************************************************
 *                    PRIMITIVA DEL ITERADOR INTERNO
 * *****************************************************************/

// Realiza un recorrido in-order por todo el árbol binario de búsqueda de manera interna utilizando una función "visitar"
// que recibe como parámetro para realizar cambios sobre cada elemento del árbol, la cual devuelve verdadero o falso en caso
// de que se quiera seguir iterando o no.
// Pre: el árbol fue creado.
// Post: realiza recorrido in-order realizando las acciones que posea la función visitar en su implementación.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

#endif // ABB_H

