#ifndef NETWORK_DYNAMICS_H_
#define NETWORK_DYNAMICS_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "definitions.h"
#include "bus.h"
#include "Random64.h"
#include "station.h"
#include "net_header.h"

/**********************************************************************************************/
/*Esta función imprime la matriz
'y' es la matriz.
'N' es el número de filas.
'M' es el número de columnas
 */
void imprimir_matriz(int *y, int N, int M);

/**********************************************************************************************/

/*Esta función hace el gillespie en los buses.
'the_bus' es el bus que va a tener la dinámica.
'i' es el número del bus.
't' es el tiempo absoluto.
'T' es el tiempo que va a durar el algoritmo.
*/
void Gillespie_buses(bus &the_bus, std::vector<agents> &aux, int &exposed, int i, double t, 
										 double seed1, double seed2, double seed3,int T=7);

/**********************************************************************************************/

/*Esta función hace el gillespie en las estaciones.
'the_station' es la estación que va a tener la dinámica.
'i' es el número del bus.
't' es el tiempo absoluto.
'T' es el tiempo que va a durar el algoritmo.
*/
bool Gillespie_estaciones(station &the_station, std::vector<agents> &aux, int &exposed, int i, double t,
													int id_NP, int count_NP, double seed1, double seed2, double seed3,int T=7);

/**********************************************************************************************/

/*Esta función baja gente de los buses.
'the_stop' es el template que me define el tipo de clase de la cual se bajan.
'the_vector' es el vector que almacena los agentes que se bajan.
'Max' número máximo de personas que pueden entrar a la estación. Es decir, número máximo de personas que se pueden bajar.
'seed' es la semilla del número aleatorio.
'prob' es el porcentaje de agentes que se bajan.(Por defecto es 0.1).
*/
template<typename T>
void GoDownFrom(T &the_stop, std::vector<agents> &the_vector, std::vector<agents> &aux, int Max, int seed, double prob=0.1)
{
	//Creo el número aleatorio que me dirá qué agente se bajará.
	Crandom ran(seed);
	
	//Creo el iterador y el index
	std::vector<agents>::iterator it;
	int index, aux_i;
	
	//Hago la dinámica de bajar la gente de los buses
	for(int i=0; i<the_stop.N(); i++){
		if(the_vector.size() == Max){break;}
		
		if(ran.r() < prob){//Miro aleatoriamente si el agente se baja			
			if(i < the_stop.Ns1.size()){//Si es un susceptible, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_stop.Ns1[i]);
				//Reviso si el agente estaba en contacto con el infectado y lo quito.
				if((it = std::find(aux.begin(), aux.end(), the_stop.Ns1[i])) != aux.end()){					
					//Si está, pues lo quito del vector.
					aux.erase(it);					
				}
				//Elimino el agente susceptible del vector. 
				the_stop.Ns1.erase( the_stop.Ns1.begin() + i);						
			}
			else if(i < the_stop.Ns1.size() + the_stop.Ns2.size()){//Si es un susceptible, lo bajo.
				aux_i = i - the_stop.Ns1.size();
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_stop.Ns2[aux_i]);
				//Reviso si el agente estaba en contacto con el infectado y lo quito.
				if((it = std::find(aux.begin(), aux.end(), the_stop.Ns2[aux_i])) != aux.end()){					
					//Si está, pues lo quito del vector.
					aux.erase(it);					
				}
				//Elimino el agente susceptible del vector. 
				the_stop.Ns2.erase( the_stop.Ns2.begin() + aux_i);						
			}
			else if(i < the_stop.Ns1.size() + the_stop.Ns2.size() + the_stop.Ni.size()){//Si es un infectado, lo bajo.				
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_stop.Ni[0]);
				//Elimino todos los vecinos del infectado y la Ego_red. Pues se va a generar una nueva red al bajar.
				std::cout << "El infectado se cambia de lugar" << '\t';
				aux.clear();				
				//Elimino el agente infectado del vector.
				the_stop.Ni.erase( the_stop.Ni.begin() + 0);	
				std::cout << "El infectado se cambia de lugar" << std::endl;
			}
			else{//Si es un expuesto, lo bajo.
				aux_i = i - (the_stop.Ns1.size() + the_stop.Ns2.size() + the_stop.Ni.size());
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_stop.Ne[aux_i]);
				//Reviso si el agente estaba en contacto con el infectado y lo quito.
				if((it = std::find(aux.begin(), aux.end(), the_stop.Ne[aux_i])) != aux.end()){					
					//Si está, pues lo quito del vector.
					aux.erase(it);					
				}
				//Elimino el agente expuesto del vector.
				the_stop.Ne.erase( the_stop.Ne.begin() + aux_i);
			}			
			//Como se quitó un agente, entonces se debe restar para poder obervarlos todos.
			i--;
		}
	}
}


/**********************************************************************************************/

#endif
