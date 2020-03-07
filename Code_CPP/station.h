#ifndef STATION_H_
#define STATION_H_

#include <vector>
#include "agents.h"

class station{
public:
	//Me dice cuánta gente hay de cada tipo
	std::vector<agents> Ni; //Infectada
	std::vector<agents> Ns; //Susceptible
	std::vector<agents> Ne; //Expuesta
	
	//Me dice de qué bus deja subir y bajar
	int bus_right_up;
	int bus_right_down;
	int bus_left_up;
	int bus_left_down;
	
	//Me dice cuántas personas máximo pueden estar en la estación
	int Nmax=1000;
	
	//Funciones
	int N(){return Ns.size() + Ni.size() + Ne.size();};
	
	//Sobrecarga de operadores
	station operator=(station b){
		this->Ni = b.Ni;
		this->Ns = b.Ns;
		this->Ne = b.Ne;
		this->bus_right_up = b.bus_right_up;
		this->bus_right_down = b.bus_right_down;
		this->bus_left_up = b.bus_left_up;
		this->bus_left_down = b.bus_left_down;
		return b;
	}
	
	friend class agents;
	station operator +(agents b){
		if(b.infected){this->Ni.push_back(b);}
		else if(b.susceptible){this->Ns.push_back(b);}
		else{this->Ne.push_back(b);}
		return *this;
	}	
};

#endif
