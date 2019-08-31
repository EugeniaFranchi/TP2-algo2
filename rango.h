#ifndef RANGO_H
#define RANGO_H


typedef struct rango rango_t;

//Crea y devuelve un rango.
rango_t* rango_crear(char* desde, char* hasta);

//Destruye el rango.
void rango_destruir(rango_t* rango);

//Devuelve el valor de inicio del rango.
char* rango_desde(rango_t* rango);

//Devuelve el valor de fin del rango.
char* rango_hasta(rango_t* rango);

#endif
