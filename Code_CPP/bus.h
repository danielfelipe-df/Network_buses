#ifndef BUS_H_
#define BUS_H_

#include <vector>
#include "agents.h"

class bus{
public:
	//Me dice cuánta gente hay de cada tipo
	std::vector<agents> Ni; //Infectada
	std::vector<agents> Ns1; //Susceptible1
	std::vector<agents> Ns2; //Susceptible2
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
	int N(){return Ns1.size() + Ns2.size() + Ni.size() + Ne.size();};
	int NS(){return Ns1.size() + Ns2.size();};
	
	void clear();
	
	//Sobrecarga de operadores
	bus operator=(bus b){
		this->Ni = b.Ni;
		this->Ns1 = b.Ns1;
		this->Ns2 = b.Ns2;
		this->Ne = b.Ne;
		this->station_up = b.station_up;
		this->station_down = b.station_down;
		return *this;
	}
	
	friend class agents;
	
	bus operator+(agents b){
		if(b.infected){this->Ni.push_back(b);}
		else if(b.susceptible1){this->Ns1.push_back(b);}
		else if(b.susceptible2){this->Ns2.push_back(b);}
		else{this->Ne.push_back(b);}
		return *this;
	}	
};

#endif