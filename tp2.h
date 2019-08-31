#ifndef TP2_H
#define TP2_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "strutil.h"
#include "abb.h"
#include "hash.h"

/* ************************************************************
 *                   PRIMITIVAS PROGRAMA
 * ***********************************************************/ 

//Procesa de forma completa un archivo de log.
//Pre: visitantes y paginas_visitadas fueron creadas.
//Post: a visitantes y a paginas_visitadas les fueron agregados
//los datos correspondientes.
bool agregar_archivo(char** parametros, abb_t* visitantes, hash_t* paginas_visitadas);

//Muestra todas las IPs en visitantes que solicitaron algún recurso 
//en el servidor dentro del rango de IPs determinado en los parametros.
//Pre: visitantes fue creado.
bool ver_visitantes(char** parametros, abb_t* visitantes);

//Muestra los n recursos más solicitados, entando n definido en los
//parametros.
//Pre: paginas_visitadas fue creado.
bool ver_mas_visitados(char** parametros, hash_t* paginas_visitadas);

#endif
