#define _GNU_SOURCE
#define POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "abb.h"
#include "hash.h"
#include "heap.h"
#include "strutil.h"
#include "tp2.h"
#include "timeutils.h"
#include "lista.h"
#include "rango.h"

#define SEP_PARAMETROS ' '
#define SEP_ERROR ' '
#define SEP_IP '.'
#define SEP_LINEA '\t'
#define MAX_CARACS 300
#define CANT_PARAM_LINEA 4
#define CANT_PARAM_AGREGAR 2
#define CANT_PARAM_VISITANTES 3
#define CANT_PARAM_MAS_VISITADOS 2
#define CANT_LOGS_MAX 5
#define DIFF_TIME_MAX 2


typedef struct pagina{
	char* url;
	size_t visitas;
}pagina_t;

typedef struct entrada{
	time_t hora;
	size_t cant;
}entrada_t;

/* ************************************************************
 *                             PAGINA
 * ***********************************************************/
pagina_t* pagina_crear(const char* url, size_t visitas){
	pagina_t* pagina = malloc(sizeof(pagina_t));
	if (!pagina){
		return NULL;
	}

	pagina->url = malloc(sizeof(char) * strlen(url)+1);
	if (!pagina->url){
		free(pagina);
		return NULL;
	}
	strcpy(pagina->url, url);
	pagina->visitas = visitas;

	return pagina;
}

void pagina_destruir(pagina_t* pagina){
	free(pagina->url);
	free(pagina);
}

/* ************************************************************
 *                             ENTRADA
 * ***********************************************************/
entrada_t* entrada_crear(time_t hora, size_t cant){
	entrada_t* entrada = malloc(sizeof(entrada_t));
	if (!entrada){
		return NULL;
	}

	entrada->hora = hora;
	entrada->cant = cant;

	return entrada;
}

void entrada_destruir(void* dato){
	entrada_t* entrada = (entrada_t*) dato;
	free(entrada);
}

/* ************************************************************
 *                   FUNCIONES AUXILIARES
 * ***********************************************************/
//Devuelve la cantidad de elementos en un arreglo.
int cant_parametros(char** parametros){
	int pos = 0;
	while (parametros[pos]){
		pos++;
	}
	return pos;
}

//Imprime el url y las visitas de la pagina dato.
bool imprimir_pagina_visitas(void* dato, void* extra){
	pagina_t* pagina = (pagina_t*) dato;
	if (!pagina){
		return false;
	}
	fprintf(stdout, "\t%s - %zu\n",pagina->url, pagina->visitas);
	return true;
}


//Recibe dos tiempos. Devuelve 0 si son iguales, 1 si dato1 es mayor
// y -1 si dato2 es mayor.
int comparar_entradas(const void* dato1, const void* dato2){
	entrada_t* entrada1 = (entrada_t*) dato1;
	entrada_t* entrada2 = (entrada_t*) dato2;
	if (difftime(entrada1->hora, entrada2->hora) > 0){
		return 1;
	}
	else if (difftime(entrada1->hora, entrada2->hora) < 0){
		return -1;
	}
	return 0;
}


//Imprime el ip, con una tabulacion antes.
bool imprimir(const char *ip, void *dato, void *extra){
	fprintf(stdout, "\t%s\n", ip);
	return true;
}

//Destruye la entrada dato.
void destruir_entrada(void* dato){
	entrada_t* entrada = (entrada_t*) dato;
	entrada_destruir(entrada);
}

//Destruye el heap dato.
void destruir_heap(void* dato){
	heap_t* heap = (heap_t*) dato;
	heap_destruir(heap, destruir_entrada);
}

//Recibe dos visitas. Devuelve -1 si dato1 es mayor, 1 si dato2 es mayor
//y 0 si son iguales.
int comparar_visitas(const void* dato1, const void* dato2){
	size_t* num1 = (size_t*) dato1;
	size_t* num2 = (size_t*) dato2;
	if (*num2 > *num1){
		return 1;
	}
	else if (*num2 == *num1){
		return 0;
	}
	return -1;
}

//Recibe dos paginas. Devuelve 1 si dato1 es mayor, -1 si dato2 es mayor
//y 0 si son iguales.
int comparar_paginas(const void* dato1,const void* dato2){
	pagina_t* pagina1 = (pagina_t*) dato1;
	pagina_t* pagina2 = (pagina_t*) dato2;

	return comparar_visitas(&pagina1->visitas, &pagina2->visitas);
}

//Recibe un heap y devuelve una lista con sus elementos.
lista_t* heap_a_lista(heap_t* heap){
	lista_t* lista = lista_crear();

	while (!heap_esta_vacio(heap)){
		void* elem = heap_desencolar(heap);
		lista_insertar_primero(lista, elem);
	}
	return lista;
}

//Imprime DoS con el ip clave.
bool imprimir_dos(const char* clave, void* dato, void* extra){
	fprintf(stdout, "DoS: %s\n", clave);
	return true;
}

//Destruye la pagina dato.
void destruir_pagina(void* dato){
	pagina_t* pagina = (pagina_t*) dato;
	pagina_destruir(pagina);
}

bool visitar(void* dato, void* extra){
	entrada_t* entrada = (entrada_t*) dato;
	fprintf(stderr, "cant: %zu, hora: %tu \n", entrada->cant, entrada->hora);
	return true;
}

//Compara los ip. Devuelve -1 si el primero es mayor, 1 si el segundo
//el mayor o 0 si son iguales.
int comparar_ips(const char* dato1, const char* dato2){
	char** ip1 = split(dato1, SEP_IP);
	char** ip2 = split(dato2, SEP_IP);
	for (int i = 0; i<4; i++){
		int ip1_int = atoi(ip1[i]);
		int ip2_int = atoi(ip2[i]);
		if (ip1_int > ip2_int){
			free_strv(ip1);
			free_strv(ip2);
			return 1;
		}
		if (ip1_int < ip2_int){
			free_strv(ip1);
			free_strv(ip2);
			return -1;
		}
	}
	free_strv(ip1);
	free_strv(ip2);
	return 0;
}

//Devuelve true si hay un DoS y false de lo contrario. 
bool chequearDoS(heap_t* tiempos, size_t cant_logs_max, double diff_time_max){
	if (diff_time_max <= 0 || heap_esta_vacio(tiempos)){
		return false;
	}
	
	entrada_t* entrada1 = heap_desencolar(tiempos);
	if (entrada1->cant >= cant_logs_max){
		heap_encolar(tiempos, entrada1);
		return true;
	}
	if (!heap_esta_vacio(tiempos)){
		size_t nuevo_cant_max = cant_logs_max - entrada1->cant;
		double nuevo_diff_max = diff_time_max - (difftime(entrada1->hora, ((entrada_t*)heap_ver_max(tiempos))->hora));
		bool rta = chequearDoS(tiempos, nuevo_cant_max, nuevo_diff_max);
		heap_encolar(tiempos, entrada1);
		return rta;
	}
	heap_encolar(tiempos, entrada1);
	return false;
}

/* ************************************************************
 *                   PRIMITIVAS PROGRAMA
 * ***********************************************************/

bool agregar_archivo(char** parametros, abb_t* visitantes, hash_t* paginas_visitadas){
	if (cant_parametros(parametros) != CANT_PARAM_AGREGAR){
		return false;
	}

	hash_t* entradas_ip = hash_crear(destruir_heap);
	abb_t* dos = abb_crear(comparar_ips,NULL);

	FILE* arch = fopen(parametros[1], "r");
	if (!arch){
		abb_destruir(dos);
		hash_destruir(entradas_ip);
		return false;
	}
	ssize_t leido = 0;
	char* linea = NULL;
	size_t largo = 0;

	while ((leido = getline(&linea, &largo, arch)) > 0 && leido != -1){
		if(linea[leido - 1] == '\n'){
			linea[leido - 1] = '\0';
		}
		char** items = split(linea, SEP_LINEA);
		if (cant_parametros(items) != CANT_PARAM_LINEA){
			free(linea);
			fclose(arch);
			free_strv(items);
			abb_destruir(dos);
			hash_destruir(entradas_ip);
			return false;
		}

		/*Arma entradas_ip*/
		time_t fecha_hora = iso8601_to_time(items[1]);
		heap_t* tiempos = (heap_t*) hash_obtener(entradas_ip, items[0]);
		if (!tiempos){
			tiempos = heap_crear(comparar_entradas);
			entrada_t* entrada = entrada_crear(fecha_hora, 1);
			heap_encolar(tiempos, entrada);
			hash_guardar(entradas_ip, items[0], tiempos);
		}
		else if(((entrada_t*)heap_ver_max(tiempos))->hora == fecha_hora){
			entrada_t* entrada = (entrada_t*) heap_ver_max(tiempos);
			entrada->cant++;
		}
		else{
			entrada_t* entrada = entrada_crear(fecha_hora, 1);
			heap_encolar(tiempos, entrada);
		}

		if (chequearDoS(tiempos, CANT_LOGS_MAX, DIFF_TIME_MAX)){
			abb_guardar(dos, items[0], items[0]);
		}

		/*Arma visitantes*/
		abb_guardar(visitantes, items[0], NULL);

		/*Arma paginas_visitadas*/
		void* dato = hash_obtener(paginas_visitadas, items[3]);
		if(!dato){
			size_t cant_dato = 1;
			size_t* cant = malloc(sizeof(size_t));
			if (!cant){
				free(linea);
				fclose(arch);
				abb_destruir(dos);
				hash_destruir(entradas_ip);
				return false;
			}
			*cant = cant_dato;
			hash_guardar(paginas_visitadas, items[3], cant);
		}
		else{
			size_t* dato_zu = (size_t*) dato;
			*dato_zu += 1;
		}

		free(linea);
		linea = NULL;
		free_strv(items);
	}
	free(linea);
	fclose(arch);
	abb_in_order(dos, imprimir_dos, NULL);
	abb_destruir(dos);
	hash_destruir(entradas_ip);
	fprintf(stdout, "OK\n");
	return true;
}


bool ver_visitantes(char** parametros, abb_t* visitantes){
	if (cant_parametros(parametros) != CANT_PARAM_VISITANTES){
		return false;
	}

	rango_t* rango = rango_crear(parametros[1], parametros[2]);
	fprintf(stdout, "Visitantes:\n");

	abb_in_order_en_rango(visitantes, imprimir, rango, NULL);

	fprintf(stdout, "OK\n");
	rango_destruir(rango);
	return true;
}


bool ver_mas_visitados(char** parametros, hash_t* paginas_visitadas) {
	if (cant_parametros(parametros) != CANT_PARAM_MAS_VISITADOS){
		return false;
	}

	int cant = atoi(parametros[1]);
	heap_t* aux = heap_crear(comparar_paginas);
	hash_iter_t* iter_visitas = hash_iter_crear(paginas_visitadas);
	for (int i = 0; i < cant && !hash_iter_al_final(iter_visitas); ++i, hash_iter_avanzar(iter_visitas)){
		const char* clave = hash_iter_ver_actual(iter_visitas);
		pagina_t* pagina = pagina_crear(clave, *(size_t*)hash_obtener(paginas_visitadas, clave));
		heap_encolar(aux, pagina);
	}

	for (int i = cant; !hash_iter_al_final(iter_visitas);i++, hash_iter_avanzar(iter_visitas)){
		const char* clave = hash_iter_ver_actual(iter_visitas);
		size_t* cant_actual = hash_obtener(paginas_visitadas, clave);
		size_t cant_a_superar = ((pagina_t*)heap_ver_max(aux))->visitas;
		if (*cant_actual > cant_a_superar){
			destruir_pagina(heap_desencolar(aux));
			pagina_t* pagina = pagina_crear(clave, *(size_t*)hash_obtener(paginas_visitadas, clave));
			heap_encolar(aux, pagina);
		}
	}

	lista_t* lista =  heap_a_lista( aux);

	fprintf(stdout, "Sitios más visitados:\n");
	lista_iterar(lista, imprimir_pagina_visitas, NULL);
	fprintf(stdout, "OK\n");
	lista_destruir(lista, destruir_pagina);
	hash_iter_destruir(iter_visitas);
	heap_destruir(aux, destruir_pagina);
	return true;
}

int main(int argc, char **argv){
	abb_t* visitantes = abb_crear(comparar_ips,NULL);
	hash_t* paginas_visitadas = hash_crear(free);

	char* linea = NULL;
	ssize_t leido;
	size_t largo = MAX_CARACS;
	bool validez;

	while ((leido = getline(&linea, &largo, stdin)) > 0 && leido > 1){
		if(linea[leido - 1] == '\n'){
			linea[leido - 1] = '\0';
		}
		char** parametros = split(linea, SEP_PARAMETROS);
		if (strcmp(parametros[0], "agregar_archivo") == 0){
			validez = agregar_archivo(parametros, visitantes, paginas_visitadas);
		}
		else if (strcmp(parametros[0], "ver_visitantes") == 0){
			validez = ver_visitantes(parametros, visitantes);
		}
		else if (strcmp(parametros[0], "ver_mas_visitados") == 0){
			validez = ver_mas_visitados(parametros, paginas_visitadas);
		}
		else{
			validez = false;
		}

		if (!validez){
			fprintf(stderr, "Error en comando %s\n", parametros[0]);
			free(linea);
			linea = NULL;
			free_strv(parametros);
			break;
		}
		free(linea);
		linea = NULL;
		free_strv(parametros);
	}
	free(linea);
	if (visitantes && paginas_visitadas){
		abb_destruir(visitantes);
		hash_destruir(paginas_visitadas);
	}
	return 0;
}
