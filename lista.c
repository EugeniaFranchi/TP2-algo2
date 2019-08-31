#include "lista.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct nodo{
	void* dato;
	struct nodo* prox;
}nodo_t;

struct lista{
	nodo_t* prim;
	nodo_t* ult;
	size_t largo;
};

struct lista_iter{
	nodo_t* ant;
	nodo_t* act;
	lista_t* lista;
};


//Crea un nodo con valor como dato.
nodo_t* nodo_crear(void* valor){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo){
		return NULL;
	}
	nodo->dato = valor;
	nodo->prox = NULL;
	return nodo;
}

/* ************************************************************
 *                           LISTA
 * ***********************************************************/ 

lista_t *lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));
	if (!lista){
		return NULL;
	}	
	lista->prim = NULL;
	lista->ult = NULL;
	lista->largo = 0;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista){
	return lista->largo == 0;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if (!nodo){
		return false;
	}
	if (lista_esta_vacia(lista)){
		lista->ult = nodo;
	}
	nodo->prox = lista->prim;
	lista->prim = nodo;
	lista->largo++;
	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if (!nodo){
		return false;
	}
	if (lista_esta_vacia(lista)){
		lista->prim = nodo;
	}
	else{
		lista->ult->prox = nodo;
	}
	lista->ult = nodo;
	lista->largo++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){
	if(lista_esta_vacia(lista)){
		return NULL;
	}
	nodo_t* nuevo_prim = lista->prim->prox;
	void* dato = lista->prim->dato;
	free(lista->prim);
	lista->prim = nuevo_prim;
	if (lista_esta_vacia(lista)){
		lista->ult = NULL;
	}
	lista->largo--;
	return dato;
}

void *lista_ver_primero(const lista_t *lista){
	if (lista_esta_vacia(lista)){
		return NULL;
	}
	return lista->prim->dato;
}

void *lista_ver_ultimo(const lista_t* lista){
	if (lista_esta_vacia(lista)){
		return NULL;
	}
	return lista->ult->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	while (!lista_esta_vacia(lista)){
		void* dato = lista_borrar_primero(lista);
		if (destruir_dato){
			destruir_dato(dato);
		}
	}
	free(lista);
}

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	nodo_t* nodo = lista->prim;
	while (nodo){
		if (visitar){
			if (!visitar(nodo->dato, extra)){
				return;
			}
		}		
		nodo = nodo->prox;
	}
}

/* ************************************************************
 *                         ITERADOR
 * ***********************************************************/ 

lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (!iter){
		return NULL;
	}
	iter->ant = NULL;
	iter->act = lista->prim;
	iter->lista = lista;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (lista_esta_vacia(iter->lista) || lista_iter_al_final(iter)){
		return false;
	}
	iter->ant = iter->act;
	iter->act = iter->act->prox;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	if (iter->act){
		return iter->act->dato;
	}
	return NULL;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return !iter->act;
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if (!nodo){
		return false;
	}
	if (iter->ant){
		iter->ant->prox = nodo;
	}
	else{
		iter->lista->prim = nodo;
	}
	if (lista_iter_al_final(iter)){
		iter->lista->ult = nodo;
	}
	nodo->prox = iter->act;
	iter->act = nodo;
	iter->lista->largo++;
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter){
	if (!iter->act){
		return NULL;
	}
	if (iter->lista->ult == iter->act){
		iter->lista->ult = iter->ant;
	}
	if (iter->ant){
		iter->ant->prox = iter->act->prox;
	}
	else if (iter->act){
		iter->lista->prim = iter->act->prox;
	}
	void* dato = iter->act->dato;
	nodo_t* nodo = iter->act;
	iter->act = iter->act->prox;
	free(nodo);
	iter->lista->largo--;
	return dato;
} 
