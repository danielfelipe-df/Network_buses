/*
 * agents.h
 *
 *  Created on: 27 Feb 2020
 *      Author: daniel
 */

#ifndef AGENTS_H_
#define AGENTS_H_

class agents {
public:
	//Número del infectado
	int number;
	
	//Defino qué tipo de persona es
	bool infected;
	bool susceptible;
	bool exposed;

	//Me controla el tiempo que lleva el personaje en el bus o estación
	int time_station;
	int time_bus;
	
	//Me dice si está en un bus o estación
	bool in_bus;
	bool in_station;
	
	//Me dice cuál es el número de bus o estación al cuál está conectado
	int location;
	
	//Función de iniciación
	agents();
	
	/*
	//Sobrecarga de operadores
	agents operator=(agents &b){		
		this->infected = b.infected;
		this->susceptible = b.susceptible;
		this->exposed = b.exposed;
		this->time_station = b.time_station;
		this->time_bus = b.time_bus;
		this->in_bus = b.in_bus;
		this->in_station = b.in_station;
		this->location = b.location;
		return *this;
	}*/
	
	//Sobrecarga de operadores
	bool operator==(const agents b){
		if(this->number == b.number){return true;}
		else{return false;}
	}
};

#endif /* AGENTS_H_ */
