#include <stdlib.h>
#include "rango.h"

struct rango{
	char* desde;
	char* hasta;
};

/* ************************************************************
 *                             RANGO
 * ***********************************************************/

rango_t* rango_crear(char* desde, char* hasta){
	rango_t* rango = malloc(sizeof(rango_t));
	if (!rango){
		return NULL;
	}

	rango->desde = desde;
	rango->hasta = hasta;

	return rango;
}

void rango_destruir(rango_t* rango){
	free(rango);
}

char* rango_desde(rango_t* rango){
	return rango->desde;
}

char* rango_hasta(rango_t* rango){
	return rango->hasta;
}
