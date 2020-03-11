#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include "network_dynamics.h"

int main(void)
{
	//Defino los vectores de buses y estaciones.
	station estaciones[NE];
	bus buses[NB];
	
	//Defino la variable que me da el identificador de agentes y la que me dice cuántos agentes hay.
	int id_NP = 5000;
	int count_NP = 5000;
	
	//Hago la matriz de adjacencia.
	int matriz[NE*NB];
	for(int i=0; i<(NE*NB); i++){matriz[i]=0;}
	//Aquí defino los enlaces entre las estaciones y los buses
	for(int i=0; i<(NE-1); i++){
		matriz[i*NB + i] = 1;
		matriz[(i+1)*NB + i] = -1;
		matriz[((NE-1)-i)*NB + ((NE-1)+i)] = 1;
		matriz[((NE-1)-(i+1))*NB + ((NE-1)+i)] = -1;
	}	
	
	imprimir_matriz(matriz,NE,2*(NE-1));
	
	//Aquí le digo a cada bus de cuál estación deja subir gente y de cuál deja bajar
	for(int i=0; i<(NE-1); i++){
		buses[i].station_up = i;
		buses[i].station_down = i+1;
		buses[(NE-1)+i].station_up = (NE-1)-i;
		buses[(NE-1)+i].station_down = (NE-1)-(i+1);
		std::cout << NE + i << ' ' << i << ' ' << i+1 << std::endl;
		std::cout << NE + (NE-1)+i << ' ' << (NE-1)-i << ' ' << (NE-1)-(i+1) << std::endl;
	}
	
	//Aquí le asigno la población inicial a las estaciones y los buses
	Crandom bran(0), sran(300), prob(12);
	int the_bus, the_station;
	agents my_agent;
	for(int i=0; i<count_NP; i++){
		my_agent.number = i;		
		if(prob.r() < 0.3){
			my_agent.in_bus = true;
			my_agent.in_station = false;
			the_bus = (int)(bran.r()*NB);
			my_agent.location = the_bus;
			buses[the_bus].Ns.push_back(my_agent);
		}
		else{
			my_agent.in_bus = false;
			my_agent.in_station = true;
			the_station = (int)(sran.r()*NE);
			my_agent.location = the_station;
			estaciones[the_station].Ns.push_back(my_agent);
		}				
	}
	
	//Escojo la persona infectada. La agrego al vector Ni y lo quito del Ns.
	the_station = (int)(prob.r()*NE);
	int infected = (int)(prob.r()*estaciones[the_station].Ns.size());
	estaciones[the_station].Ni.push_back( estaciones[the_station].Ns[infected]);
	estaciones[the_station].Ni[0].susceptible = false;
	estaciones[the_station].Ni[0].infected = true;
	estaciones[the_station].Ns.erase( estaciones[the_station].Ns.begin() + infected);	
	
	//Digo cuántos pasos de tiempo va a dar la simulación
	double T=200;
	
	//Creo los arreglos auxiliares para el intercambio de personas	
	std::vector<agents> go_up_to_bus[NB];
	std::vector<agents> go_down_from_bus[NB];
	bus aux_buses[NB];
	
	//Declaro las variables auxiliares que dependen del bus.
	int sta_down, sta_up;
	
	//Genero la variable para imprimir los datos.
	std::ofstream fout;	
	
	//Abro el archivo para imprimir los susceptibles.
	fout.open("Datos/susceptibles.csv", std::ofstream::app);
	//Imprimo los datos
	fout << 0;
	for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ns.size();}
	for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ns.size();}
	fout << std::endl;
	fout.close();
			
	//Abro el archivo para imprimir los susceptibles.
	fout.open("Datos/expuestos.csv", std::ofstream::app);
	//Imprimo los datos
	fout << 0;
	for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ne.size();}
	for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ne.size();}	
	fout << std::endl;
	fout.close();
			
	//Abro el archivo para imprimir los susceptibles.
	fout.open("Datos/infectados.csv", std::ofstream::app);
	//Imprimo los datos
	fout << 0;
	for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ni.size();}
	for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ni.size();}
	fout << std::endl;
	fout.close();
	
	//Le doy los valores actuales de cada bus a los arreglos auxiliares.
	for(int j=0; j<NB; j++){aux_buses[j] = buses[j];}

	//Genero la dinámica 
	for(double t=0; t<T; t++){	
		std::cout << "Time: " << t << '\t';
		//Hago el Gillespie en cada uno de los buses y estaciones
		for(int j=0; j<NB; j++){if(buses[j].Ni.size() != 0 && buses[j].Ns.size() != 0){Gillespie_buses(buses[j],j,t);}}
		for(int j=0; j<NE; j++){Gillespie_estaciones(estaciones[j],j,id_NP,count_NP,t);}
		
		//Le doy los valores actuales de cada bus a los arreglos auxiliares.
		for(int j=0; j<NB; j++){
			aux_buses[j].Ni = buses[j].Ni;
			aux_buses[j].Ne = buses[j].Ne;
			aux_buses[j].Ns = buses[j].Ns;
		}
		
		//Hago el intercambio de personas de buses a estaciones.
		for(int j=0; j<NB; j++){
			//Miró a qué estación se baja y a qué estación se sube.			
			sta_down = aux_buses[j].station_down;
			sta_up = aux_buses[j].station_up;			
			
			//Guardo las personas que se bajan del bus. Las quito del bus, sin agregarlas a la estación.
			GoDownFrom(aux_buses[j],go_down_from_bus[j],estaciones[sta_down].Nmax-estaciones[sta_down].N(),sta_down*100 + j*NB);			
			
			//Guardo las personas que se suben al bus. Las quito de la estación, sin agregarlas al bus.
			GoDownFrom(estaciones[sta_up],go_up_to_bus[j],aux_buses[j].Nmax-aux_buses[j].N(),sta_up*NB+j*NE);
		}
		
		std::cout << t << '\t';
		//Agrego las personas que bajé del bus a la estación. Y que bajé de la estación al bus.
		for(int j=0; j<NB; j++){
			//Del bus a la estación
			sta_down = aux_buses[j].station_down;
			for(int k=0; k<go_down_from_bus[j].size(); k++){
				estaciones[sta_down] = estaciones[sta_down] + go_down_from_bus[j][k];
			}
			go_down_from_bus[j].clear();
			
			//De la estación al bus.
			sta_up = aux_buses[j].station_up;
			for(int k=0; k<go_up_to_bus[j].size(); k++){
				aux_buses[j] = aux_buses[j] + go_up_to_bus[j][k];
			}
			go_up_to_bus[j].clear();
		}
		
		std::cout << t << std::endl;
		std::cout << count_NP << std::endl;
		
		//Abro el archivo para imprimir los susceptibles.
		fout.open("Datos/susceptibles.csv", std::ofstream::app);
		//Imprimo los datos
		fout << t+1;
		for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ns.size();}
		for(int j=0; j<NB; j++){fout << '\t' << aux_buses[j].Ns.size();}		
		fout << std::endl;
		fout.close();
		
		//Abro el archivo para imprimir los susceptibles.
		fout.open("Datos/expuestos.csv", std::ofstream::app);
		//Imprimo los datos
		fout << t+1;
		for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ne.size();}
		for(int j=0; j<NB; j++){fout << '\t' << aux_buses[j].Ne.size();}		
		fout << std::endl;
		fout.close();
		
		//Abro el archivo para imprimir los susceptibles.
		fout.open("Datos/infectados.csv", std::ofstream::app);
		//Imprimo los datos
		fout << t+1;
		for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ni.size();}
		for(int j=0; j<NB; j++){fout << '\t' << aux_buses[j].Ni.size();}		
		fout << std::endl;
		fout.close();
		
		//Hacer el intercambio de pasajeros de un bus a otro.
		for(int j=0; j<NB; j++){
			buses[(j+1)%NB].Ni = aux_buses[j].Ni;
			buses[(j+1)%NB].Ne = aux_buses[j].Ne;
			buses[(j+1)%NB].Ns = aux_buses[j].Ns;
		}		
	}	
	
	for(int i=0; i<NB; i++){std::cout << buses[i].N() << '\t';}
	std::cout << std::endl;	
	for(int i=0; i<NE; i++){std::cout << estaciones[i].N() << '\t';}
	std::cout << std::endl;	
	
	return 0;	
}

