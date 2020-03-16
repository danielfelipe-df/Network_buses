#ifndef BUS_H_
#define BUS_H_

#include <vector>
#include "agents.h"

class bus{
public:
	//Me dice cuánta gente hay de cada tipo
	std::vector<agents> Ni; //Infectada
	std::vector<agents> Ns; //Susceptible
	std::vector<agents> Ne; //Expuesta	
	
	//Me dice de qué estación deja subir y bajar
	int station_up; //Estación de la cuál se suben personas
	int station_down; //Estación de la cuál se bajan personas
	
	//Me dice la posición del bus en este momento.
	int location;
	
	//Me dice la dirección del bus
	int direction;
	
	//Me dice cuántas personas máximo pueden estar en la estación
	int Nmax=300;
	
	//Funciones
	int N(){return Ns.size() + Ni.size() + Ne.size();};
	
	void clear();
	
	//Sobrecarga de operadores
	bus operator=(bus b){
		this->Ni = b.Ni;
		this->Ns = b.Ns;
		this->Ne = b.Ne;
		this->station_up = b.station_up;
		this->station_down = b.station_down;
		return *this;
	}
	
	friend class agents;
	
	bus operator+(agents b){
		if(b.infected){this->Ni.push_back(b);}
		else if(b.susceptible){this->Ns.push_back(b);}
		else{this->Ne.push_back(b);}
		return *this;
	}	
};

#endif