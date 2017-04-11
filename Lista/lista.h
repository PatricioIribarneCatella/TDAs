#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct lista lista_t;

typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear();

// Devuelve verdadero o falso, según si la lista tiene o no elementos dentro de ella.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Inserta un nuevo elemento al principio de la lista. Devuelve verdadero si la acción de pudo concretar o falso en caso contrario.
// Pre: la lista fue creada.
// Post: se agreg un nuevo elemento a lista. Valor se encuentra al principio de la misma.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Inserta un nuevo elemento al final de la lista. Devuelve verdadero si la acción se pudo concretar o falso en caso contrario.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista. Valor se encuentra al final de la misma.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista posee elementos, se quita de ella el primero y se devuelve el valor ccorrespondiente.
// De estar vacía, se devuelve NULL.
// Pre: la lista fue creada.
// Post: devuelve el valor del primer elemento si la lista no estaba vacia.
void *lista_borrar_primero(lista_t *lista);

// Devuelve el valor del primer elemento sin alterar la lista, en caso de estar vacia devuelve NULL.
// Pre: la lista fue creada.
// Post: devuelve el valor del primer elmento si la lista no estaba vacia.
void *lista_ver_primero(const lista_t *lista);

// Devuelve el largo de la lista, en cuanto a cantidad de elementos.
// Pre: la lista fue creada.
// Post: devuelve el largo de la lista, cero en caso de que esté vacia.
size_t lista_largo(const lista_t *lista);

// Destruye la lista como así tambien los elementos que contenga.
// Destruir_dato es una función que se recibe por parámetro y que es capáz de destruir los elementos de la lista.
// En caso de no utilizarlo se recibe NULL como parámetro.
// Pre: la lista fue creada.
// Post: la lista fue destruida y los elementos que ccontenía también.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

// Crea un nuevo iterador.
// Pre: la lista fue creada.
// Post: se creó un nuevo iterador.
lista_iter_t *lista_iter_crear(const lista_t *lista);

// Se mueve a lo largo de la lista de a un elemento por vez.
// Pre: la lista y el iterador fueron creados.
// Post: se movió una posición hacia adelante en la lista y devuelve verdadero si lo pudo concretar, o falso en caso contrario.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el valor del elemento en la lista al cual está apuntadndo el iterador en ese momento.
// Pre: la lista y el iterador fueron creados.
// Post: devuelve el valor del elemento actual.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Se fija si el iterador está al final de la lista.
// Pre: la lista y el iterador fueron creados.
// Post: devuelve verdadero en caso de que se encuentre al final de la lista o falso en caso contrario.
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador creado anteriormmente.
// Pre: el iterador fue creado.
// Post: el iterador fue destruído.
void lista_iter_destruir(lista_iter_t *iter);

/* ******************************************************************
 *                PRIMITIVAS DEL ITERADOR EN LA LISTA
 * *****************************************************************/

// Recibe como parámetros la lista, el iterador y un dato a guardar a en la lista y lo inserta en la posición actual del ieterador.
// Teniendo en cuenta que se lo va a insertar detrás del elemento al cual se encontraba apuntando el iterador en ese momento.
// Pre: la lista y el ieterador fueron creados.
// Post: se insertó el elemento en la lista devolviendo verdadero si se pudo concretar la acción o falso en caso contrario.
bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato);

// Recibe como parámetros la lista, el iterador y devuelve el dato del elemento al cual está apuntando el iterador en ese momento.
// Quita el elemento de la lista.
// Pre: la lista y el iterador fueron creados.
// Post: devuelve el valor del elemento y lo quita de la lista.
void *lista_borrar(lista_t *lista, lista_iter_t *iter);


/* ******************************************************************
 *                 PRIMITIVA DEL ITERADOR INTERNO
 * *****************************************************************/

// Itera sobre la lista de manera interna recibiendo como parámetros la lista, una funcion visitar que realiza acciones sobre cada elemento que
// devuelve verdadero si se debe seguir iterando o falso en caso contrario, y
// un dato extra por si se necesita crear por ejemplo un índice.
// Pre: la lista fue creada.
// Post: realiza las acciones sobre cada elemento que posea la función visitar.
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);

#endif // LISTA_H
