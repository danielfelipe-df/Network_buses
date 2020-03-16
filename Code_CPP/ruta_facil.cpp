#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include "network_dynamics.h"
#include "net_header.h"

int main(void)
{	
	//Defino el generador de semillas.
	Crandom g_seed(1890);
	const int MY_MAX = 457830;
	
	//Defino la variable que después me contará el número total de susceptibles, expuestos e infectados en el sistema.
	int suma;
	//Variable que me lleva el conteo de todos los expuestos.
	int exposed;
	
	//Defino los vectores de buses y estaciones.
	station estaciones[NE];
	bus buses[NB];
	
	//Defino la variable que me da el identificador de agentes y la que me dice cuántos agentes hay.
	int id_NP, count_NP;
	
	Crandom bran((int)(g_seed.r()*MY_MAX)), sran((int)(g_seed.r()*MY_MAX)), prob((int)(g_seed.r()*MY_MAX));
	int the_bus, the_station;
	
	int infected;
	
	Crandom ran_net((int)(g_seed.r()*MY_MAX));
	std::vector<agents> inf_net;
	
	network the_net;
	
	double T;
	
	std::ofstream fout;
	
	//Creo los arreglos auxiliares para el intercambio de personas	
	std::vector<agents> go_up_to_bus[NB];
	std::vector<agents> go_down_from_bus[NB];
	bus aux_buses[NB];
	
	//Declaro las variables auxiliares que dependen del bus.
	int sta_down, sta_up;
	
	for(int n=0; n<300; n++){
		//Reinicio el número de expuestos.
		exposed = 0;
	
		//Defino la variable que me da el identificador de agentes y la que me dice cuántos agentes hay.
		id_NP = 3000;
		count_NP = 3000;
	
		//Aquí le digo a cada bus de cuál estación deja subir gente y de cuál deja bajar
		for(int i=0; i<(NE-1); i++){
			buses[i].station_up = i;
			buses[i].station_down = i+1;
			buses[(NE-1)+i].station_up = (NE-1)-i;
			buses[(NE-1)+i].station_down = (NE-1)-(i+1);
		}
	
		//Aquí le asigno la población inicial a las estaciones y los buses		
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
		infected = (int)(prob.r()*estaciones[the_station].Ns.size());
		estaciones[the_station].Ni.push_back( estaciones[the_station].Ns[infected]);
		estaciones[the_station].Ni[0].susceptible = false;
		estaciones[the_station].Ni[0].infected = true;
		estaciones[the_station].Ns.erase( estaciones[the_station].Ns.begin() + infected);
	
		//Hago la Ego-red en donde está ubicado el infectado.
		//Primero identifico cuáles se van a conectar con él.		
		for(int i=0; i<estaciones[the_station].Ns.size(); i++){
			if(ran_net.r() < 0.05){inf_net.push_back(estaciones[the_station].Ns[i]);}
		}
		
		//Ahora creo la red.		
		Ego_red(the_net, inf_net.size()+1);		
	
		//Digo cuántos pasos de tiempo va a dar la simulación
		T=80;
	
		//Genero la dinámica 
		for(double t=0; t<T; t+=7){			
			
			//Hago el Gillespie en cada uno de los buses y estaciones
			for(int j=0; j<NB; j++){if(buses[j].Ni.size() != 0 && buses[j].Ns.size() != 0){Gillespie_buses(buses[j],the_net,inf_net,exposed,j,t);}}
			for(int j=0; j<NE; j++){Gillespie_estaciones(estaciones[j],the_net,inf_net,exposed,j,t,id_NP,count_NP);}			
		
			//Le doy los valores actuales de cada bus a los arreglos auxiliares.
			for(int j=0; j<NB; j++){aux_buses[j] = buses[j];}			
			
			//Hago el intercambio de personas de buses a estaciones.
			for(int j=0; j<NB; j++){				
				//Miró a qué estación se baja y a qué estación se sube.			
				sta_down = aux_buses[j].station_down;
				sta_up = aux_buses[j].station_up;			
			
				//Guardo las personas que se bajan del bus. Las quito del bus, sin agregarlas a la estación.
				GoDownFrom(aux_buses[j],go_down_from_bus[j],inf_net,the_net,estaciones[sta_down].Nmax-estaciones[sta_down].N(),(int)(g_seed.r()*MY_MAX));			
			
				//Guardo las personas que se suben al bus. Las quito de la estación, sin agregarlas al bus.
				GoDownFrom(estaciones[sta_up],go_up_to_bus[j],inf_net,the_net,aux_buses[j].Nmax-aux_buses[j].N(),(int)(g_seed.r()*MY_MAX));
			}
			
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
			
			//Reviso si no hay individuos conectados al infectado. Porque, si es así, él se salio del sistema o cambió de lugar.
			if(inf_net.size() == 0){
				//Reviso si está en un bus
				for(int j=0; j<NB; j++){
					if(aux_buses[j].Ni.size() != 0){// Si esto es distinto de cero, es porque hay un infectado.
						//Agrego susceptibles al vector.
						for(int k=0; k<aux_buses[j].Ns.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(aux_buses[j].Ns[k]);}
						}
						//Agrego expuestos al vector.
						for(int k=0; k<aux_buses[j].Ne.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(aux_buses[j].Ne[k]);}
						}					
						//Hago la Ego_red con los agentes.
						Ego_red(the_net, inf_net.size()+1);
						//Rompo el ciclo, pues ya encontré donde estaba.
						break;
					}
				}
			
				//Reviso si está en un bus
				for(int j=0; j<NE; j++){
					if(estaciones[j].Ni.size() != 0){// Si esto es distinto de cero, es porque hay un infectado.
						//Agrego susceptibles al vector.
						for(int k=0; k<estaciones[j].Ns.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(estaciones[j].Ns[k]);}
						}	
						//Agrego expuestos al vector.
						for(int k=0; k<estaciones[j].Ne.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(estaciones[j].Ne[k]);}
						}
						//Hago la Ego_red con los agentes.
						Ego_red(the_net, inf_net.size()+1);
						//Rompo el ciclo, pues ya encontré donde estaba.
						break;
					}
				}
			}
			
			//Reviso si no hay individuos conectados al infectado. Porque, si es así, él se salio del sistema.
			if(inf_net.size() == 0){break;}

			//Hacer el intercambio de pasajeros de un bus a otro.
			for(int j=0; j<NB; j++){
				buses[(j+1)%NB].Ni = aux_buses[j].Ni;
				buses[(j+1)%NB].Ne = aux_buses[j].Ne;
				buses[(j+1)%NB].Ns = aux_buses[j].Ns;
			}

			//std::cout << "Número de conectados al infectado: " << inf_net.size() << std::endl;
		}
		
		//Reinicio todos los contenedores y variables.
		for(int i=0; i<NB; i++){buses[i].clear();		aux_buses[i].clear();}
		for(int i=0; i<NE; i++){estaciones[i].clear();}		
		the_net.clear();
		inf_net.clear();
		
		//Imprimo el número de expuestos
		fout.open("corridas_3000.csv", std::ofstream::app);
		fout << n+1 << '\t' << exposed << std::endl;
		fout.close();
	}
	
	return 0;	
}

