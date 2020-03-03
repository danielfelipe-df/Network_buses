/*
 * bus_station.h
 *
 *  Created on: 28 Feb 2020
 *      Author: daniel
 */

#ifndef BUS_STATION_H_
#define BUS_STATION_H_

#include <vector>

class bus{
public:
	//Me dice cuánta gente hay de cada tipo
	std::vector<int> Ni; //Infectada
	std::vector<int> Ns; //Susceptible
	std::vector<int> Ne; //Expuesta
	
	//Me dice de qué estación deja subir y bajar
	int station_up;
	int station_down;
	
	//Me dice cuántas personas máximo pueden estar en la estación
	int Nmax;
	
	//Funciones
	int N(){return Ns.size() + Ni.size() + Ne.size();};
	
	//Sobrecarga de operadores
	bus operator=(bus &b){
		this->Ni = b.Ni;
		this->Ns = b.Ns;
		this->Ne = b.Ne;
		this->station_up = b.station_up;
		this->station_down = b.station_down;
		return b;
	}
};


class station{
public:
	//Me dice cuánta gente hay de cada tipo
	std::vector<int> Ni; //Infectada
	std::vector<int> Ns; //Susceptible
	std::vector<int> Ne; //Expuesta
	
	//Me dice de qué bus deja subir y bajar
	int bus_right_up;
	int bus_right_down;
	int bus_left_up;
	int bus_left_down;
	
	//Me dice cuántas personas máximo pueden estar en la estación
	int Nmax;
	
	//Funciones
	int N(){return Ns.size() + Ni.size() + Ne.size();};
	
	//Sobrecarga de operadores
	station operator=(station &b){
		this->Ni = b.Ni;
		this->Ns = b.Ns;
		this->Ne = b.Ne;
		this->bus_right_up = b.bus_right_up;
		this->bus_right_down = b.bus_right_down;
		this->bus_left_up = b.bus_left_up;
		this->bus_left_down = b.bus_left_down;
		return b;
	}
};


#endif