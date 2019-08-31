#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stdlib.h>
#include "rango.h"

/* ************************************************************
 *              DEFINICION DE LOS TIPOS DE DATO
 * ***********************************************************/ 

typedef struct abb abb_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb_iter abb_iter_t;

/* ************************************************************
 *                     PRIMITIVAS ABB
 * ***********************************************************/ 

//Crea un abb y lo devuelve. Devuelve NULL en caso de error.
//Post: el abb fue creado.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

//Guarda un dato con su clave en un abb. Devuelve false en caso de error.
//Pre: el abb fue creado.
//Post: el abb tiene un nuevo elemento.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

//Borra el elemento marcado por la clave y devuelve el dato.
//Devuelve NULL en caso de error.
//Pre: el abb fue creado.
//Post: el elemento fue borrado.
void *abb_borrar(abb_t *arbol, const char *clave);

//Devuelve el dato indicado por la clave. Devuelve NULL en aso de error.
//Pre: el abb fue creado.
void *abb_obtener(const abb_t *arbol, const char *clave);

//Devuelve true si el elemento indicado por la clave pertenece al abb.
//Pre: el abb fue creado.
bool abb_pertenece(const abb_t *arbol, const char *clave);

//Devuelve la cantidad de elementos en el abb.
//Pre: el abb fue creado.
size_t abb_cantidad(abb_t *arbol);

//Destruye el abb.
//Pre: el abb fue creado.
//Post: sus elementos fueron destruidos.
void abb_destruir(abb_t *arbol);

//Aplica la funcion visitar a cada elemento del abb en orden creciente.
//Pre: el abb fue creado.
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

//Aplica la funcion visitar a cada elemento del abb, que cumpla con el rango, en orden creciente.
//Pre: el abb fue creado.
void abb_in_order_en_rango(abb_t *arbol, bool visitar(const char *, void *, void *), rango_t* rango, void* extra);

/* ************************************************************
 *                   PRIMITIVAS ITERADOR
 * ***********************************************************/ 

//Crea un iterador y lo devuelve.
//Pre: el abb fue creado.
//Post: el iterador fue creado.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

//Avanza a la siguiente posicion.
//Pre: el iterador fue creado.
bool abb_iter_in_avanzar(abb_iter_t *iter);

//Devuelve la clave del nodo actual.
//Pre: el iterador fue creado.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

//Devuelve true si el iterador esta al final del abb.
//Pre: el iterador fue creado.
bool abb_iter_in_al_final(const abb_iter_t *iter);

//Destruye el iterador.
//Pre: el iterados fue creado.
//Post: el iterador fue destruido.
void abb_iter_in_destruir(abb_iter_t* iter);


#endif
