#include "pila.h"
#include <stdlib.h>
#include <stdbool.h>

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/
#define CAPACIDAD_INICIAL 10
#define FACTOR_AUMENTO 2 //Redimension al apilar
#define CRITERIO_ACHICAR 4 //Redimension al desapilar


pila_t* pila_crear(void){
	pila_t* pila = malloc(sizeof(pila_t));
	
	if (!pila){
		return NULL;
	}
	
	pila->datos = malloc(CAPACIDAD_INICIAL * sizeof(void*));

    if (pila->datos == NULL) {
        free(pila);
        return NULL;
    }
	
	pila->cantidad = 0;
	pila->capacidad = CAPACIDAD_INICIAL;
	return pila;
}


void pila_destruir(pila_t *pila){
	free(pila->datos);
	free(pila);
}


bool pila_esta_vacia(const pila_t *pila){
	return pila->cantidad == 0;
}


//Cambia el tamanio de la pila por tam. Devuelve false si hay un error.
//Pre: la pila fue creada.
//Post: la pila tiene un tamanio tam.
bool pila_redimensionar(pila_t *pila, size_t tam){
	void** datos = realloc(pila->datos, sizeof(void*)* tam);
	if (datos == NULL){
		return false;
	}
	pila->datos = datos;
	pila->capacidad = tam;
	return true;
}


bool pila_apilar(pila_t *pila, void* valor){
		
	if (pila->cantidad >= pila->capacidad){
		if(! pila_redimensionar(pila, pila->capacidad * FACTOR_AUMENTO)){
			return false;
		}
	}
	pila->datos[pila->cantidad] = valor;
	pila->cantidad++;
	return true;
}


void* pila_ver_tope(const pila_t *pila){
	if (pila_esta_vacia(pila)){
		return NULL;
	}
	return pila->datos[pila->cantidad - 1];
}


void* pila_desapilar(pila_t *pila){
	if (pila_esta_vacia(pila)){
		return NULL;
	}
	if (pila->cantidad * CRITERIO_ACHICAR <= pila->capacidad && pila->capacidad/FACTOR_AUMENTO >= CAPACIDAD_INICIAL){
		pila_redimensionar(pila, pila->capacidad / FACTOR_AUMENTO);
	}
	void* valor = pila_ver_tope(pila);
	pila->cantidad--;
	return valor;
}
