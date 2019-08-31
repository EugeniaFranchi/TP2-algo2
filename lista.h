#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

/* ************************************************************
 *              DEFINICION DE LOS TIPOS DE DATO
 * ***********************************************************/ 
       
typedef struct lista lista_t;

typedef struct lista_iter lista_iter_t;

/* ************************************************************
 *                      PRIMITIVAS LISTA
 * ***********************************************************/ 

//Crea una lista.
//Post: devuelve una lista vacia.
lista_t *lista_crear(void);

//Devuelve verdadero si no tiene elementos. De lo contrario devuelve falso.
//Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

//Inserta un elemento en la primera posicion de la lista.
//Devuelve false en caso de error.
//Pre: la lista fue creada.
//Post: el primer elemento es el de dato.
bool lista_insertar_primero(lista_t *lista, void *dato);

//Inserta un elemento en la ultima posicion de la lista.
//Devuelve false en caso de error.
//Pre: la lista fue creada.
//Post: el ultimo elemento es el de dato.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Borra el primer elemento de la lista y lo devuelve.
//En caso de estar vacia, devuelve NULL.
//Pre: la lista fue creada.
//Post: el primer elemento fue borrado.
void *lista_borrar_primero(lista_t *lista);

//Devuelve el primer elemento de la lista.
//En caso de estar vacia, devuelve NULL.
//Pre: la lista fue creada.
void *lista_ver_primero(const lista_t *lista);

//Devuelve el ultimo elemento de la lista.
//En caso de estar vacia, devuelve NULL.
//Pre: la lista fue creada.
void *lista_ver_ultimo(const lista_t* lista);

//Devuelve el largo de la lista.
//Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);

//Destruye la lista. Si se recibe una funcion destruir_dato,
//la llama para cada dato de la lista.
//Pre: la lista fue creada. destruir_dato es una funcion capaz de
//destruir los datos de la lista, o NULL en caso que no se utilice.
//Post: la lista y sus elementos fueron destruidos.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

//Aplica la funcion visitar a cada elemento de la lista. En caso
//de ser NULL, no se aplicara.
//Pre: la lista fue creada.
//Post: los datos cambiaron de acuerdo a la funcion.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* ************************************************************
 *                    PRIMITIVAS ITERADOR
 * ***********************************************************/ 

//Crea el iterador. En caso de error devuelve NULL.
//Pre: la lista fue creada.
//Post: devuelve un iterador con el primer elemento de la lista como
//posicion actual o NULL si esta vacia.
lista_iter_t *lista_iter_crear(lista_t *lista);

//Avanza una posicion en la lista. Devuelve false en caso de error.
//Pre: el iterador fue creado.
bool lista_iter_avanzar(lista_iter_t *iter);

//Devuelve el elemento de la lista que se encuentra en la posicion actual.
//Pre: el iterador fue creado.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Devuelve verdadero si el iterador esta en la ultima posicion de la lista.
//Pre: el iterador fue creado.
bool lista_iter_al_final(const lista_iter_t *iter);

//Destruye el iterador.
//Pre: el iterador fue creado.
//Post: el iterador fue destruido.
void lista_iter_destruir(lista_iter_t *iter);

//Inserta un dato en la posicion actual.
//Pre: el iterador fue creado.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Borra el elemento actual del iterador. Devuelve el dato, o NULL de
//no existir.
//Pre: el iterador fue creado.
//Post: se borra el elemento.
void *lista_iter_borrar(lista_iter_t *iter);

/* ************************************************************
 *                          PRUEBAS
 * ***********************************************************/ 

//Realiza las pruebas del iterador y lista.
void pruebas_lista_alumno(void);

#endif
