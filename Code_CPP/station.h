#ifndef STATION_H_
#define STATION_H_

#include <vector>
#include "agents.h"
#include "bus.h"

class station{
public:
	//Me dice cuánta gente hay de cada tipo
	std::vector<agents> Ni; //Infectada
	std::vector<agents> Ns1; //Susceptible1
	std::vector<agents> Ns2; //Susceptible2
	std::vector<agents> Ne; //Expuesta
	
	//Me dice de qué bus deja subir y bajar
	int location;
	
	//Me dice cuántas personas máximo pueden estar en la estación
	int Nmax=1000;
	
	//Funciones
	int N(){return Ns1.size() + Ns2.size() + Ni.size() + Ne.size();};
	int NS(){return Ns1.size() + Ns2.size();};
	
	void clear();
	
	//Sobrecarga de operadores
	station operator=(station b){
		this->Ni = b.Ni;
		this->Ns1 = b.Ns1;
		this->Ns2 = b.Ns2;
		this->Ne = b.Ne;		
		return b;
	}
	
	friend class agents;
	station operator +(agents b){
		if(b.infected){this->Ni.push_back(b);}
		else if(b.susceptible1){this->Ns1.push_back(b);}
		else if(b.susceptible2){this->Ns2.push_back(b);}
		else{this->Ne.push_back(b);}
		return *this;
	}
	
	friend class bus;
	station operator+(const bus b){
		this->Ni.insert(this->Ni.end(), b.Ni.begin(), b.Ni.end());
		this->Ns1.insert(this->Ns1.end(), b.Ns1.begin(), b.Ns1.end());
		this->Ns2.insert(this->Ns2.end(), b.Ns2.begin(), b.Ns2.end());
		this->Ne.insert(this->Ne.end(), b.Ne.begin(), b.Ne.end());
		return *this;
	}
};

#endif
