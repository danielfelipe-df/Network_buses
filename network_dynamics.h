#ifndef NETWORK_DYNAMICS_H_
#define NETWORK_DYNAMICS_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "definitions.h"
#include "bus.h"
#include "Random64.h"
#include "station.h"

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
void Gillespie_buses(bus &the_bus, int i, double t, int T=1);

/**********************************************************************************************/

/*Esta función hace el gillespie en las estaciones.
'the_station' es la estación que va a tener la dinámica.
'i' es el número del bus.
't' es el tiempo absoluto.
'T' es el tiempo que va a durar el algoritmo.
*/
void Gillespie_estaciones(station &the_station, int i, double t, int id_NP, int count_NP, int T=1);

/**********************************************************************************************/

/*Esta función baja gente de los buses.
'the_stop' es el template que me define el tipo de clase de la cual se bajan.
'the_vector' es el vector que almacena los agentes que se bajan.
'Max' número máximo de personas que pueden entrar a la estación. Es decir, número máximo de personas que se pueden bajar.
'seed' es la semilla del número aleatorio.
'prob' es el porcentaje de agentes que se bajan.(Por defecto es 0.1).
*/
template<typename T>
void GoDownFrom(T &the_stop, std::vector<agents> &the_vector, int Max, int seed, double prob=0.1)
{
	//Creo el número aleatorio que me dirá qué agente se bajará.
	Crandom ran(seed);	
	
	//Hago la dinámica de bajar la gente de los buses
	for(int i=0; i<the_stop.N(); i++){
		if(the_vector.size() == Max){break;}
		
		if(ran.r() < prob){//Miro aleatoriamente si el agente se baja			
			if(i < the_stop.Ns.size()){//Si es un susceptible, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_stop.Ns[i]);
				//Elimino el agente susceptible del vector. 
				the_stop.Ns.erase( the_stop.Ns.begin() + i);						
			}
			else if(i < the_stop.Ns.size() + the_stop.Ni.size()){//Si es un infectado, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_stop.Ni[0]);
				//Elimino el agente infectado del vector.
				the_stop.Ni.erase( the_stop.Ni.begin() + 0);	
				std::cout << "El infectado se cambia de lugar" << std::endl;
			}
			else{//Si es un expuesto, lo bajo.	
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_stop.Ne[i-(the_stop.Ns.size() + the_stop.Ni.size())]);
				//Elimino el agente expuesto del vector.
				the_stop.Ne.erase( the_stop.Ne.begin() + i-(the_stop.Ns.size() + the_stop.Ni.size()));
			}			
			//Como se quitó un agente, entonces se debe restar para poder obervarlos todos.
			i--;
		}
	}
}


/**********************************************************************************************/

#endif
