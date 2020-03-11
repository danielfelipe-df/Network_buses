#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

//Defino el número de estaciones y buses;
const int NE = 10;
const int NB = 2*(NE-1);

//Defino las variables globales de la epidemia
const double beta = 1.35e-3; //Datos de Wuhan (minutos^{-1})
const double Sa = 1e-3; //Dato inventado (minutos^{-1})

#endif