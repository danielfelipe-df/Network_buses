#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include "Random64.h"

//Defino el número de estaciones y buses;
const int NE = 10;
const int NB = 2*(NE-1);
//const int NB = 6;

//Defino las variables globales de la epidemia
const double beta1 = 1.35e-3; //Datos de Wuhan beta-prom (minutos^{-1})
const double beta2 = 2*beta1; //Datos de Wuhan beta-prom (minutos^{-1})
//const double beta = 8.68e-4; //Datos de Wuhan beta-min (minutos^{-1})
//const double beta = 4.66e-3; //Datos de Wuhan beta-max (minutos^{-1})
//const double Sa = 1e-3; //Dato inventado (minutos^{-1})
const double Sa = 14.18; //Dato inventado (minutos^{-1})

//Constante para los números aleatorios.
const int MY_MAX = 457830;

#endif