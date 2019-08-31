#include "abb.h"
#include "pila.h"
#include "rango.h"
#include "strutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define SEP_IP '.'

typedef struct abb_nodo{
	struct abb_nodo* izq;
	struct abb_nodo* der;
	char* clave;
	void* dato;
} abb_nodo_t;
	
struct abb{
	abb_nodo_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cant;
};

struct abb_iter{
	pila_t* nodos;
};

/* ************************************************************
 *                        AUXILIARES
 * ***********************************************************/

//Destruye los nodos del abb a partir del nodo nodo.
//Post: los nodos fueron destruidos.
void destruir(abb_nodo_t* nodo, abb_destruir_dato_t destruir_dato){
	if (!nodo){
		return;
	}
	if (nodo->izq){
		destruir(nodo->izq, destruir_dato);
	}
	if (nodo->der){
		destruir(nodo->der, destruir_dato);
	}
	if (destruir_dato) {
		destruir_dato(nodo->dato);
	}
	free(nodo->clave);
	free(nodo);
}

//Intercambia la clave y valor de dos nodos.
bool abb_nodo_swap(abb_nodo_t* nodo1, abb_nodo_t* nodo2){
	if (!nodo1 || !nodo2){
		return false;
	}
	char* clave = nodo1->clave;
	void* dato = nodo1->dato;
	nodo1->clave = nodo2->clave;
	nodo1->dato = nodo2->dato;
	nodo2->clave = clave;
	nodo2->dato = dato;
	return true;
}

//Compara las cadenas de numeros separadas por puntos. Devuelve -1 si el primero es mayor, 1 si el segundo
//el mayor o 0 si son iguales.
int comparar_numeros_separados(const char* dato1, const char* dato2){
	char** ip1 = split(dato1, SEP_IP);
	char** ip2 = split(dato2, SEP_IP);
	for (int i = 0; i<4; i++){
		int ip1_int = atoi(ip1[i]);
		int ip2_int = atoi(ip2[i]);
		if (ip1_int > ip2_int){
			free_strv(ip1);
			free_strv(ip2);
			return -1;
		}
		if (ip1_int < ip2_int){
			free_strv(ip1);
			free_strv(ip2);
			return 1;
		}
	}
	free_strv(ip1);
	free_strv(ip2);
	return 0;
}

/* ************************************************************
 *                        ABB_NODO
 * ***********************************************************/

//Crea el abb_nodo.
abb_nodo_t* abb_nodo_crear(const char* clave, void* dato){
	abb_nodo_t* abb_nodo = malloc(sizeof(abb_nodo_t));
	if (!abb_nodo){
		return NULL;
	}
	char* datos = malloc(sizeof(char) * strlen(clave)+1);
	if (!datos){
		return NULL;
	}
	strcpy(datos, clave);
	abb_nodo->clave = datos;
	abb_nodo->dato = dato;
	abb_nodo->der = NULL;
	abb_nodo->izq = NULL;
	
	return abb_nodo;
}

//Apunta nodo a el nodo correspondiente a la la clave y a padre a su padre.
//Si no se encuentra la clave, nodo no cambia y padre apunta a quien deberia
//ser su padre.
void abb_nodo_buscar_nodo_y_padre(abb_nodo_t* raiz, const char* clave, abb_comparar_clave_t cmp, abb_nodo_t** nodo, abb_nodo_t** padre){
	if (!raiz){
		return;
	}
	if (cmp(raiz->clave, clave) == 0){
		*nodo = raiz;
		return;
	}
	else if (cmp(raiz->clave, clave) > 0){
		*padre = raiz;
		if (raiz->izq){
			abb_nodo_buscar_nodo_y_padre(raiz->izq, clave, cmp, nodo, padre);
		}
		return;
	}
	else{
		*padre = raiz;
		if (raiz->der){
			abb_nodo_buscar_nodo_y_padre(raiz->der, clave, cmp, nodo, padre);
		}
		return;
	}
}


//Inserta el nodo en el lugar correspondiete. Si la clave existe,
//cambia el dato.

bool abb_nodo_insertar(abb_nodo_t* nodo, const char* clave, void* dato, abb_comparar_clave_t cmp){
	abb_nodo_t* hijo = NULL;
	abb_nodo_t* padre = NULL;
	abb_nodo_buscar_nodo_y_padre(nodo, clave, cmp, &hijo, &padre);
	if (!padre){
		padre = nodo;
	}
	if(hijo){
		hijo->dato = dato;
	}
	else if(cmp(padre->clave, clave) > 0){
		padre->izq = abb_nodo_crear(clave, dato);
	}
	else{
		padre->der = abb_nodo_crear(clave, dato);
	}
	
	return true;
}

//Devuelve el nodo correspondiente a la clave, o NULL en caso de error.
abb_nodo_t* abb_nodo_buscar(abb_nodo_t* nodo, const char* clave, abb_comparar_clave_t cmp){
	abb_nodo_t* hijo = NULL;
	abb_nodo_t* padre = NULL;
	abb_nodo_buscar_nodo_y_padre(nodo, clave, cmp, &hijo, &padre);
	
	return hijo;
}

//Busca el padre de un nodo y lo devuelve. Si el nodo es la raiz, devuelve NULL.
//Pre: el nodo debe pertenecer al arbol.
abb_nodo_t* abb_nodo_buscar_padre(abb_nodo_t* nodo, abb_nodo_t* padre, const char* clave, abb_comparar_clave_t cmp){
	abb_nodo_t* hijo = NULL;
	abb_nodo_buscar_nodo_y_padre(nodo, clave, cmp, &hijo, &padre);
	
	return padre;
}

//Devuelve el nodo con menor valor.
abb_nodo_t* abb_nodo_minimo(abb_nodo_t* nodo) {
	abb_nodo_t* act = nodo;
	while (act->izq != NULL){
		act = act->izq;
	}
	return act;
}

//Devuelve el nodo con mayor valor.
abb_nodo_t* abb_nodo_maximo(abb_nodo_t* nodo) {
	abb_nodo_t* act = nodo;
	while (act->der != NULL){
		act = act->der;
	}
	return act;
}

//Borra y devuelve el dato de correspondiente a la clave.
void *abb_nodo_borrar(abb_t* arbol, const char *clave, abb_nodo_t *padre, abb_nodo_t *nodo){
	void* dato = nodo->dato;
	
	if (!nodo->der && !nodo->izq){  //caso ningun hijo
		if (!padre){
			arbol->raiz = NULL;
		}
		else if (padre->der == nodo){
			padre->der = NULL;
		}
		else{
			padre->izq = NULL;
		}
		free(nodo->clave);
		free(nodo);
		arbol->cant--;
	}
	
	else if (!nodo->der){ //caso hijo izq
		if (!padre) {
			arbol->raiz = nodo->izq;
		}
		else if (padre->der == nodo){
			padre->der = nodo->izq;
		}
		else{
			padre->izq = nodo->izq;
		}
		free(nodo->clave);
		free(nodo);
		arbol->cant--;
	}
	
	else if (!nodo->izq){ //caso hijo der
		if (padre == NULL) {
			arbol->raiz = nodo->der;
		}
		else if (padre->der == nodo){
			padre->der = nodo->der;
		}
		else{
			padre->izq = nodo->der;
		}
		free(nodo->clave);
		free(nodo);
		arbol->cant--;
	}
	
	else{   //caso 2 hijos
		abb_nodo_t* minimo = abb_nodo_minimo(nodo->der);
		abb_nodo_t* padre_min = abb_nodo_buscar_padre(nodo, NULL, minimo->clave, arbol->cmp);
		abb_nodo_swap(nodo, minimo);
		abb_nodo_borrar(arbol, clave, padre_min, minimo);
	}
	
	return dato;
}

//Aplica la funcion visitar a cada nodo conectado al nodo pasado por
//parametro en orden creciente.
bool abb_nodo_in_order(abb_nodo_t* nodo, bool visitar(const char *, void *, void *), void *extra){
	if (!nodo){
		return true;
	}
	if (!abb_nodo_in_order(nodo->izq, visitar, extra)){
		return false;
	}

	if (!visitar(nodo->clave, nodo->dato, extra)){
		return false;
	}
	
	if (!abb_nodo_in_order(nodo->der, visitar, extra)){
		return false;
	}
	return true;
}

bool abb_nodo_in_order_en_rango(abb_nodo_t* nodo, bool visitar(const char *, void *, void *), rango_t* rango, void* extra){
	if (!nodo){
		return true;
	}
	
	bool mayor_a_desde = comparar_numeros_separados((char*)nodo->clave, rango_desde(rango)) <= 0;
	bool menor_a_hasta = comparar_numeros_separados((char*)nodo->clave, rango_hasta(rango)) >= 0;
	
	if (mayor_a_desde && !abb_nodo_in_order_en_rango(nodo->izq, visitar, rango, extra)){
		return false;
	}

	if (mayor_a_desde && menor_a_hasta && !visitar(nodo->clave, nodo->dato, extra)){
		return false;
	}
	
	if (menor_a_hasta && !abb_nodo_in_order_en_rango(nodo->der, visitar, rango, extra)){
		return false;
	}
	return true;
}

/* ************************************************************
 *                            ABB
 * ***********************************************************/
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* abb = malloc(sizeof(abb_t));
	if (!abb){
		return NULL;
	}
	
	abb->cant = 0;
	abb->cmp = cmp;
	abb->destruir_dato = destruir_dato;
	abb->raiz = NULL;
	
	return abb;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	if (!arbol->raiz){
		arbol->raiz = abb_nodo_crear(clave, dato);
		arbol->cant++;
		return true;
	}
	
	abb_nodo_t* hijo = NULL;
	abb_nodo_t* padre = NULL;
	abb_nodo_buscar_nodo_y_padre(arbol->raiz, clave, arbol->cmp, &hijo, &padre);
	if (!hijo){
		if (!abb_nodo_insertar(padre, clave, dato, arbol->cmp)){
			return false;
		}
		arbol->cant++;
	}
	else{
		if (arbol->destruir_dato){
			arbol->destruir_dato(hijo->dato);
		}
		hijo->dato = dato;
	}
	return true;
}


void *abb_borrar(abb_t* arbol, const char *clave){
	if (!arbol || !arbol->raiz){
		return NULL;
	}
	abb_nodo_t* hijo = NULL;
	abb_nodo_t* padre = NULL;
	abb_nodo_buscar_nodo_y_padre(arbol->raiz, clave, arbol->cmp, &hijo, &padre);
	if (!hijo){
		return NULL;
	}
	return abb_nodo_borrar(arbol, clave, padre, hijo);
}


void *abb_obtener(const abb_t *arbol, const char *clave){
	if (!arbol->raiz){
		return NULL;
	}
	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz, clave, arbol->cmp);
	if (!nodo){
		return NULL;
	}
	return nodo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	return abb_nodo_buscar(arbol->raiz, clave, arbol->cmp);
}

size_t abb_cantidad(abb_t *arbol){
	return arbol->cant;
}


void abb_destruir(abb_t *arbol){
	if (arbol->raiz) {
		destruir(arbol->raiz,arbol->destruir_dato);
	}
	free(arbol);
}


void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	if (!arbol || !arbol->raiz){
		return;
	}
	
	abb_nodo_in_order(arbol->raiz, visitar, extra);
}

void abb_in_order_en_rango(abb_t *arbol, bool visitar(const char *, void *, void *), rango_t* rango, void* extra){
	if (!arbol || !arbol->raiz){
		return;
	}
	
	abb_nodo_in_order_en_rango(arbol->raiz, visitar, rango, extra);
}

/* ************************************************************
 *                          ITERADOR
 * ***********************************************************/

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if (!iter){
		return NULL;
	}
	
	iter->nodos = pila_crear();
	if (!iter->nodos) {
		return NULL;
	}

	abb_nodo_t* nodo = arbol->raiz;	
	while (nodo){
		pila_apilar(iter->nodos, nodo);
		nodo = nodo->izq;
	}
	
	return iter;	
}


bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter->nodos);
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)){
		return false;
	}
	
	abb_nodo_t* nodo = pila_desapilar(iter->nodos);
	abb_nodo_t* derecho = nodo->der;
	if (derecho){
		pila_apilar(iter->nodos, derecho);
	
		while (derecho->izq){
			pila_apilar(iter->nodos, derecho->izq);
			derecho = derecho->izq;
		}
	}
	return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)){
		return false;
	}
	
	abb_nodo_t* nodo = (abb_nodo_t*)pila_ver_tope(iter->nodos);
	return nodo->clave;
}


void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->nodos);
	free(iter);
}
