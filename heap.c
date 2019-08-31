#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include <stdio.h>

#define TAMANIO_INICIAL 10
#define CRITERIO_AUMENTO 0.7
#define CRITERIO_ACHICAR 0.3
#define FACTOR_AUMENTO 2
#define FACTOR_ACHICAR 0.5

struct heap{
	cmp_func_t cmp;
	void** arr;
	size_t cant;
	size_t tam;
	};

/* ************************************************************
 *                    FUNCIONES AUXILIARES
 * ***********************************************************/ 

//Devuelve una copia del arreglo.
void **copiar_arreglo(void* arreglo[], size_t tam){
	void** copia = malloc(sizeof(void*) * tam);
	if (!copia){
		return NULL;
	}
	for(size_t i = 0; i<tam; i++){
		copia[i] = arreglo[i];
	}
	return copia;
}

//Devuelve la posicion en la que deberia encontrarse el hijo
//izquierdo de un nodo de heap.
size_t posicion_hijo_izq (size_t pos_actual) {
	return (2*pos_actual+1);
} 

//Devuelve la posicion en la que deberia encontrarse el hijo
//derecho de un nodo de heap.
size_t posicion_hijo_der (size_t pos_actual) {
	return (2*pos_actual+2);
} 

//Devuelve la posicion en la que deberia encontrarse el hijo
//izquierdo de un nodo de heap.
size_t posicion_padre (size_t  pos_actual) {
	return ((pos_actual-1)/2);
} 

//Intercambia las posiciones de pos_x y pos_y.
void swap (void* arreglo[], size_t pos_x, size_t pos_y) {
	void* aux_x = arreglo[pos_x];
	arreglo[pos_x] = arreglo[pos_y];
	arreglo[pos_y] = aux_x;
}

//Verifica que el nodo en pos_nodo sea menor al padre. Si no lo es,
//lo ordena, ascendiendo su posicion.
void upheap(void *elementos[], cmp_func_t cmp, size_t pos_nodo){
	size_t pos_padre = posicion_padre(pos_nodo);
	if ((int)pos_padre < 0){
		return;
	}
	if (cmp(elementos[pos_nodo], elementos[pos_padre]) > 0){
		swap(elementos, pos_nodo, pos_padre);
		upheap(elementos, cmp, pos_padre);
	}
}

//Devuelve la posicion del nodo con valor valor en un arreglo.
size_t hijo_mayor(void *elementos[], cmp_func_t cmp, size_t hijo1, size_t hijo2){
	if (cmp(elementos[hijo1], elementos[hijo2]) > 0){
		return hijo1;
	}
	else{
		return hijo2;
	}
}

//Verifica que el nodo en pos_nodo sea menor al padre. Si no lo es,
//lo ordena, descendiendo su posicion.
void downheap(void *elementos[], size_t cant, cmp_func_t cmp, size_t pos_nodo){
	size_t hijo_der = posicion_hijo_der(pos_nodo);
	size_t hijo_izq = posicion_hijo_izq(pos_nodo);
	if (hijo_izq >= cant){
		return;
	}
	
	size_t pos_hijo_mayor;
	if (hijo_der >= cant){
		pos_hijo_mayor = hijo_izq;
	}
	else{
		pos_hijo_mayor = hijo_mayor(elementos, cmp, hijo_der, hijo_izq);
	}
	if (cmp(elementos[pos_nodo], elementos[pos_hijo_mayor]) < 0){
		swap(elementos, pos_nodo, pos_hijo_mayor);
		downheap(elementos, cant, cmp, pos_hijo_mayor);
	}
}

//Aplica downheap a cada nodo de un arreglo.
void heapify(void *elementos[], size_t cant, cmp_func_t cmp){
	for (int pos_nodo = (int)cant-1 ;pos_nodo >= 0; pos_nodo--){
		downheap(elementos, cant, cmp, (size_t)pos_nodo);
	}
}

//Ordena un arreglo de heap.
void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
	heapify(elementos, cant, cmp);
	for (int ultimo = (int)cant - 1; ultimo > 0; ultimo--){
		swap(elementos, 0, (size_t) ultimo);
		downheap(elementos, (size_t) ultimo, cmp, 0);
		
	}
}

//Cambia el tamanio del heap a nuevo_tam.
bool heap_redimensionar(heap_t* heap, size_t nuevo_tam){
	void** arr_nuevo = realloc(heap->arr, nuevo_tam * sizeof(void*));
    if (!arr_nuevo) {
        return false;
    }

    heap->arr = arr_nuevo;
    heap->tam = nuevo_tam;
    return true;
}
/* ************************************************************
 *                     PRIMITIVAS HEAP
 * ***********************************************************/ 

heap_t *heap_crear(cmp_func_t cmp){
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap){
		return NULL;
	}
	void** datos = malloc(sizeof(void*) * TAMANIO_INICIAL);
	if (!datos){
		free(heap);
		return NULL;
	}
	
	heap->cmp = cmp;
	heap->arr = datos;
	heap->cant = 0;
	heap->tam = TAMANIO_INICIAL;
	
	return heap;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap){
		return NULL;
	}
	
	heap->cmp = cmp;
	heap->arr = copiar_arreglo(arreglo, n);
	if (!heap->arr){
		free(heap);
		return NULL;
	}
	heap->cant = n;
	heap->tam = n * 2;
	heapify(heap->arr, n, cmp);
	
	return heap;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	if(destruir_elemento){
		for (size_t i = heap->cant-1; (int)i>=0; i--){
			destruir_elemento(heap->arr[i]);
		}
	}
	free(heap->arr);
	free(heap);
}

size_t heap_cantidad(const heap_t *heap){
	return heap->cant;
}

bool heap_esta_vacio(const heap_t *heap){
	return !heap_cantidad(heap);
}

bool heap_encolar(heap_t *heap, void *elem){
	if (!elem){
		return false;
	}
	if ((double)heap->cant/(double)heap->tam >= CRITERIO_AUMENTO){
		if (!heap_redimensionar(heap, heap->tam*FACTOR_AUMENTO)){
			return false;
		}
	}
	
	heap->arr[heap->cant] = elem;
	heap->cant++;
	
	upheap(heap->arr, heap->cmp, heap->cant - 1);
	return true;
}

void *heap_ver_max(const heap_t *heap){
	if (heap_esta_vacio(heap)){
		return NULL;
	}
	return heap->arr[0];
}

void *heap_desencolar(heap_t *heap){
	if (((double)heap->cant / (double)heap->tam < CRITERIO_ACHICAR) && ((double)heap->tam * FACTOR_ACHICAR >= TAMANIO_INICIAL)){
		heap_redimensionar(heap, (size_t)((double)heap->tam * FACTOR_ACHICAR));
	}
	
	if (heap_esta_vacio(heap)){
		return NULL;
	}
	void* nodo = heap->arr[0];
	swap(heap->arr, 0, heap->cant - 1);
	heap->cant--;
	
	downheap(heap->arr, heap->cant, heap->cmp, 0);
	return nodo;
}

