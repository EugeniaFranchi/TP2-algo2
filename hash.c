#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAMANIO_INICIAL 10
#define CRITERIO_AUMENTO 0.7
#define CRITERIO_ACHICAR 0.3
#define FACTOR_AUMENTO 2
#define FACTOR_ACHICAR 0.5

typedef enum estado_campo {VACIO,BORRADO,OCUPADO} estado_t ;
typedef struct hash_campo hash_campo_t ;

struct hash_campo{
	char* clave;
	void* valor;
	estado_t estado_campo ;
} ;

struct hash{
	hash_campo_t* tabla;
	size_t tam;
	size_t cant;
	size_t cant_borrados;
	hash_destruir_dato_t destruir_dato;
};

struct hash_iter{
	hash_campo_t* act;
	const hash_t* hash;
};

/* ************************************************************
 *                     FUNCIONES AUXILIARES
 * ***********************************************************/


size_t hashing (const char* word, size_t hashtable_size){
	int hash_adress = 0;
	for (int counter = 0; word[counter] != '\0'; counter++){
		hash_adress = word[counter] + (hash_adress << 6) + (hash_adress <<16) - hash_adress;
	}
	return (size_t) (hash_adress%hashtable_size);
}

//Devuelve la primera posicion vacia a partir de la posicion que arroja la funcion
//si  no la encuentra devuelve -1
int hallar_pos_vacia(hash_campo_t* tabla, size_t tam, int clave_h){
	int pos = clave_h ;
	while(pos != clave_h -1){
		if(tabla[pos].estado_campo == VACIO)
	 		return pos ;
	 	//cuando buscó hasta el final de la tabla vuelve a empezar desde el ppio
	 	if(pos != tam-1)
	 		pos ++ ;
	 	else
	 		pos = 0 ;
	 }
	 return -1 ;
}

//Devuelve verdadero si esta vacio y false de lo contrario.
bool hash_esta_vacio(const hash_t* hash){
	return hash->cant == 0;
}

//Devuelve la siguiente posicion ocupada del hash, o -1 en caso de no haber.
int hallar_pos_ocupada(const hash_t* hash, size_t inicio){
	if (hash_esta_vacio(hash)){
		return -1;
	}
	for (int pos = (int)inicio; pos < (int)hash->tam; pos++){
		if(hash->tabla[pos].estado_campo == OCUPADO){
			return pos;
		}
	}
	return -1;
}

//Busca en la tabla la clave que le pasan por parametro
//si la encuentra, devuelve la posicion, sino devuelve -1
size_t posicion_clave(const hash_t* hash, const char*clave){
	int clave_hasheada = (int)hashing(clave, hash->tam) ;
	bool primero = true;
	for (int actual = clave_hasheada; hash->tabla[actual%hash->tam].estado_campo == OCUPADO || hash->tabla[actual%hash->tam].estado_campo == BORRADO; actual++){
		if (primero){
			primero = false;
		}
		else if(actual%hash->tam == clave_hasheada){
			break;
		}
		if(strcmp(hash->tabla[actual%hash->tam].clave ,clave) == 0 && hash->tabla[actual%hash->tam].estado_campo == OCUPADO)
			return actual%hash->tam ;
	}
	return -1 ;
}


//Destruye el contenido de la tabla.
void destruir_contenido_tabla(hash_campo_t* tabla, size_t tam, hash_destruir_dato_t destruir_dato){
	for(int i = 0 ; i < tam ; i++){
		if(tabla[i].estado_campo == VACIO)
			continue ;
		else{
			free(tabla[i].clave) ;
		}
		if(destruir_dato && tabla[i].estado_campo == OCUPADO){
			destruir_dato(tabla[i].valor) ;
		}		
	}
}


//Cambia el tamanio del hash por tam. Devuelve false si hay un error.
//Pre: el hash fue creado.
//Post: el hash tiene un tamanio tam.
bool hash_redimensionar(hash_t *hash, size_t tam){
	hash_campo_t* nueva_tabla = malloc(sizeof(hash_campo_t)* tam );
	if (!nueva_tabla){
		return false;
	}
	for(int i = 0 ; i < tam ; i++)
		nueva_tabla[i].estado_campo = VACIO ;
	
	hash_campo_t* tabla_vieja = hash->tabla;
	size_t tamanio = hash->tam;
	hash->tabla = nueva_tabla;
	hash->tam = tam;
	size_t cant = hash->cant;
	hash->cant = 0;
	for (size_t pos = 0; pos < tamanio; pos++){
		if (tabla_vieja[pos].estado_campo == OCUPADO){
			hash_guardar(hash, tabla_vieja[pos].clave, tabla_vieja[pos].valor);
			free(tabla_vieja[pos].clave) ;
		}
		if (tabla_vieja[pos].estado_campo == BORRADO){
			free(tabla_vieja[pos].clave) ;
		}
	}
	hash->cant = cant;
	hash->cant_borrados = 0;
	free(tabla_vieja);
	return true;
}


/* ************************************************************
 *                             HASH
 * ***********************************************************/ 
hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc( sizeof(hash_t));
	if (!hash){
		return NULL;
	}
	
	hash_campo_t* datos = malloc( sizeof(hash_campo_t) * TAMANIO_INICIAL);
	if (!datos){
		free(hash);
		return NULL;
	}
	
	hash->tabla = datos;
	hash->cant = 0;
	hash->cant_borrados = 0;
	hash->tam = TAMANIO_INICIAL;
	hash->destruir_dato = destruir_dato;
	for(int i = 0 ; i < hash->tam ; i++)
		hash->tabla[i].estado_campo = VACIO ;
	return hash;
}

void hash_destruir(hash_t *hash){
	destruir_contenido_tabla(hash->tabla, hash->tam, hash->destruir_dato) ;
	free(hash->tabla) ;
	free(hash);
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){	
	if (((double)(hash->cant + hash->cant_borrados))/(double)hash->tam >= (double)CRITERIO_AUMENTO){
		hash_redimensionar(hash, hash->tam * FACTOR_AUMENTO);
	}
		
	size_t pos_clave = posicion_clave(hash, clave) ;
	
	if(pos_clave == -1){
		int clave_hasheada = (int)hashing(clave, hash->tam) ;
		int pos_vacia = hallar_pos_vacia(hash->tabla, hash->tam, clave_hasheada) ;
		
		if(pos_vacia == -1)
			return false ;
		
		hash->tabla[pos_vacia].clave = malloc(sizeof(char) * strlen(clave)+1) ;
		if(!hash->tabla[pos_vacia].clave)
			return false ;
		strcpy(hash->tabla[pos_vacia].clave,clave);
		hash->tabla[pos_vacia].valor = dato ;
		hash->tabla[pos_vacia].estado_campo = OCUPADO ;
		hash->cant++;
	}
	
	else{
		if(hash->destruir_dato)
			hash->destruir_dato(hash->tabla[pos_clave].valor) ;
		hash->tabla[pos_clave].valor = dato ;
	}
	
	return true ;
}

void *hash_borrar(hash_t *hash, const char *clave){
	size_t pos = posicion_clave(hash, clave) ;
	if(pos == -1)
		return NULL ;
	//la clave existe en la tabla

	void* dato = hash->tabla[pos].valor ;
	hash->tabla[pos].estado_campo = BORRADO ;
	hash->cant -- ;
	hash->cant_borrados++;
	
	if (((double)hash->cant)/(double)hash->tam <= CRITERIO_ACHICAR && (((double)hash->tam) * FACTOR_ACHICAR >= TAMANIO_INICIAL)){
		size_t nuevo_tam =  (size_t) ((double)hash->tam * FACTOR_ACHICAR) ;
		hash_redimensionar(hash, nuevo_tam);
	}
	return dato ;
}


void *hash_obtener(const hash_t *hash, const char *clave){
	size_t pos = posicion_clave(hash, clave) ;
	if(pos == -1)
		return NULL ;

	return hash->tabla[pos].valor ;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t pos = posicion_clave(hash, clave) ;
	return pos != -1 ;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cant ;
}


/* ************************************************************
 *                         ITERADOR
 * ***********************************************************/ 

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));
	if (!iter){
		return NULL;
	}
	
	if (hallar_pos_ocupada(hash, 0) == -1){
		iter->act = NULL;
	}
	else{
		iter->act = &hash->tabla[hallar_pos_ocupada(hash, 0)];
	}
	iter->hash = hash;
	return iter;
}


bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_iter_al_final(iter)){
		return false;
	}
	
	size_t pos = posicion_clave(iter->hash, iter->act->clave);
	if (hallar_pos_ocupada(iter->hash, (size_t)pos+1) == -1){
		iter->act = NULL;
	}
	else{
		iter->act = &iter->hash->tabla[hallar_pos_ocupada(iter->hash, (size_t)pos+1)];
	}
	return true;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if (!iter->act){
		return NULL;
	}
	return iter->act->clave;
}


bool hash_iter_al_final(const hash_iter_t *iter){
	return !iter->act;
}


void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
