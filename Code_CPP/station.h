#ifndef STATION_H_
#define STATION_H_

#include <vector>
#include "agents.h"
#include "bus.h"

class station{
public:
	//Me dice cuánta gente hay de cada tipo
	std::vector<agents> Ni; //Infectada
	std::vector<agents> Ns; //Susceptible
	std::vector<agents> Ne; //Expuesta
	
	//Me dice de qué bus deja subir y bajar
	int location;
	
	//Me dice cuántas personas máximo pueden estar en la estación
	int Nmax=1000;
	
	//Funciones
	int N(){return Ns.size() + Ni.size() + Ne.size();};
	
	void clear();
	
	//Sobrecarga de operadores
	station operator=(station b){
		this->Ni = b.Ni;
		this->Ns = b.Ns;
		this->Ne = b.Ne;		
		return b;
	}
	
	friend class agents;
	station operator +(agents b){
		if(b.infected){this->Ni.push_back(b);}
		else if(b.susceptible){this->Ns.push_back(b);}
		else{this->Ne.push_back(b);}
		return *this;
	}
	
	friend class bus;
	station operator+(const bus b){
		this->Ni.insert(this->Ni.end(), b.Ni.begin(), b.Ni.end());
		this->Ns.insert(this->Ns.end(), b.Ns.begin(), b.Ns.end());
		this->Ne.insert(this->Ne.end(), b.Ne.begin(), b.Ne.end());
		return *this;
	}
};

#endif
