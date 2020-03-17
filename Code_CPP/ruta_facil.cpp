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
	
  double T;
	
  std::ofstream fout;
	
  //Creo los arreglos auxiliares para el intercambio de personas	
  std::vector<agents> go_up_to_bus[NB];
  std::vector<agents> go_down_from_bus[NB];
  bus aux_buses[NB];
  
  //Declaro las variables auxiliares que dependen del bus.
  int sta_down, sta_up;
  
  //Variable que me dice si el infectado se salio o no.
  bool in_inf;
	
  for(int n=0; n<10; n++){
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
    for(int i=0; i<count_NP; i++){
    	agents my_agent(g_seed.r());
    	my_agent.number = i;		
    	if(prob.r() < 0.3){    		
    		my_agent.in_bus = true;    
    		my_agent.in_station = false;
    		the_bus = (int)(bran.r()*NB);
    		my_agent.location = the_bus;
    		buses[the_bus] = buses[the_bus] + my_agent;
			}
			else{
				my_agent.in_bus = false;
				my_agent.in_station = true;
				the_station = (int)(sran.r()*NE);
				my_agent.location = the_station;
				estaciones[the_station] = estaciones[the_station] + my_agent;
			}				
		}
		
		//Escojo la persona infectada. La agrego al vector Ni y lo quito del Ns.
		the_station = (int)(prob.r()*NE);
		infected = (int)(prob.r()*estaciones[the_station].Ns1.size());
		estaciones[the_station].Ni.push_back( estaciones[the_station].Ns1[infected]);
		estaciones[the_station].Ni[0].susceptible1 = false;
		estaciones[the_station].Ni[0].infected = true;
		estaciones[the_station].Ns1.erase( estaciones[the_station].Ns1.begin() + infected);
	
		//Hago la Ego-red en donde está ubicado el infectado.
		//Primero identifico cuáles se van a conectar con él.		
		for(int i=0; i<estaciones[the_station].Ns1.size(); i++){
			if(ran_net.r() < 0.05){inf_net.push_back(estaciones[the_station].Ns1[i]);}
		}
		for(int i=0; i<estaciones[the_station].Ns2.size(); i++){
			if(ran_net.r() < 0.05){inf_net.push_back(estaciones[the_station].Ns2[i]);}
		}
	
		//Digo cuántos pasos de tiempo va a dar la simulación
		T=80;
	
		//Genero la dinámica 
		for(double t=0; t<T; t+=7){
			std::cout << "Time: " << t << std::endl;
			
			std::cout << "Número de conectados al infectado: " << inf_net.size() << '\t';
			//Hago el Gillespie en cada uno de los buses y estaciones
			for(int j=0; j<NB; j++){
				if(buses[j].Ni.size() != 0 && buses[j].NS() != 0){
					Gillespie_buses(buses[j],inf_net,exposed,j,t,(double)g_seed.r(),(double)g_seed.r(),(double)g_seed.r());
				}
			}
			for(int j=0; j<NE; j++){
				in_inf = Gillespie_estaciones(estaciones[j],inf_net,exposed,j,t,id_NP,count_NP,(double)g_seed.r(),(double)g_seed.r(),(double)g_seed.r());
				if(!in_inf){break;}
			}
			
			if(!in_inf){break;}
		
			//Le doy los valores actuales de cada bus a los arreglos auxiliares.
			for(int j=0; j<NB; j++){aux_buses[j] = buses[j];}			
			
			//Hago el intercambio de personas de buses a estaciones.
			for(int j=0; j<NB; j++){				
				//Miró a qué estación se baja y a qué estación se sube.			
				sta_down = aux_buses[j].station_down;
				sta_up = aux_buses[j].station_up;			
			
				//Guardo las personas que se bajan del bus. Las quito del bus, sin agregarlas a la estación.
				GoDownFrom(aux_buses[j],go_down_from_bus[j],inf_net,estaciones[sta_down].Nmax-estaciones[sta_down].N(),(int)(g_seed.r()*MY_MAX));			
			
				//Guardo las personas que se suben al bus. Las quito de la estación, sin agregarlas al bus.
				GoDownFrom(estaciones[sta_up],go_up_to_bus[j],inf_net,aux_buses[j].Nmax-aux_buses[j].N(),(int)(g_seed.r()*MY_MAX));
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
						//Agrego susceptibles 1 al vector.
						for(int k=0; k<aux_buses[j].Ns1.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(aux_buses[j].Ns1[k]);}
						}
						//Agrego susceptibles 2 al vector.
						for(int k=0; k<aux_buses[j].Ns2.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(aux_buses[j].Ns2[k]);}
						}
						//Agrego expuestos al vector.
						for(int k=0; k<aux_buses[j].Ne.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(aux_buses[j].Ne[k]);}
						}							
						//Rompo el ciclo, pues ya encontré donde estaba.
						break;
					}
				}
			
				//Reviso si está en un bus
				for(int j=0; j<NE; j++){
					if(estaciones[j].Ni.size() != 0){// Si esto es distinto de cero, es porque hay un infectado.
						//Agrego susceptibles 1 al vector.
						for(int k=0; k<estaciones[j].Ns1.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(estaciones[j].Ns1[k]);}
						}
						//Agrego susceptibles 2 al vector.
						for(int k=0; k<estaciones[j].Ns2.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(estaciones[j].Ns2[k]);}
						}
						//Agrego expuestos al vector.
						for(int k=0; k<estaciones[j].Ne.size(); k++){
							if(ran_net.r() < 0.05){inf_net.push_back(estaciones[j].Ne[k]);}
						}						
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
				buses[(j+1)%NB].Ns1 = aux_buses[j].Ns1;
				buses[(j+1)%NB].Ns2 = aux_buses[j].Ns2;
			}
			
		}
		
		//Reinicio todos los contenedores y variables.
		for(int i=0; i<NB; i++){buses[i].clear();		aux_buses[i].clear();}
		for(int i=0; i<NE; i++){estaciones[i].clear();}		
		inf_net.clear();
		
		
		//Imprimo el número de expuestos
		fout.open("betas_2.csv", std::ofstream::app);
		fout << n+1 << '\t' << exposed << std::endl;
		fout.close();
	}
	
	return 0;	
}

