#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include "agents.h"
#include "Random64.h"
#include "bus_station.h"

/**********************************************************************************************/
//Defino las variables globales de la epidemia
const double beta = 0.5;
const double Sa = 0.1;

//Defino la variable global del número de agentes, el vector de agentes y el identificador de agentes.
int NP = 5000;

/**********************************************************************************************/
/*Esta función imprime la matriz
'y' es la matriz.
'N' es el número de filas.
'M' es el número de columnas
 */
void imprimir_matriz(int *y, int N, int M);

/*Esta función elimina el susceptible
'stop' es la estación o bus.
'agent' es el número asociado al agente.
'ilocation' es el número asociado al bus o estación en el que se encuentra el agente.
*/
template<typename T>
void erase_susceptible(T *stop, int agent, int ilocation)
{
	int i;
	for(i=0; i<stop[ilocation].Ns.size(); i++){if(stop[ilocation].Ns[i] == agent){break;}}
	stop[ilocation].Ns.erase( stop[ilocation].Ns.begin() + i);
}

/*Esta función elimina el expuesto
'stop' es la estación o bus.
'agent' es el número asociado al agente.
'ilocation' es el número asociado al bus o estación en el que se encuentra el agente.
*/
template<typename T>
void erase_exposed(T *stop, int agent, int ilocation)
{
	int i;
	for(i=0; i<stop[ilocation].Ne.size(); i++){if(stop[ilocation].Ns[i] == agent){break;}}
	stop[ilocation].Ne.erase( stop[ilocation].Ns.begin() + i);
}

/*Esta función hace el gillespie en los buses.
'the_bus' es el bus que va a tener la dinámica.
'i' es el número del bus.
't' es el tiempo absoluto.
'T' es el tiempo que va a durar el algoritmo.
*/
void Gillespie_buses(bus &the_bus, int i, double t, int T=1);

/*Esta función hace el gillespie en las estaciones.
'the_station' es la estación que va a tener la dinámica.
'i' es el número del bus.
't' es el tiempo absoluto.
'T' es el tiempo que va a durar el algoritmo.
*/
void Gillespie_estaciones(station &the_station, int i, double t, int T=1);

/*Esta función baja gente de los buses.
'the_bus' es el bus del que se bajan.
'the_vector' es el vector que almacena los agentes que se bajan.
'Max' número máximo de personas que pueden entrar a la estación. Es decir, número máximo de personas que se pueden bajar.
'seed' es la semilla del número aleatorio.
'prob' es el porcentaje de agentes que se bajan.(Por defecto es 0.1).
*/
void GoDownFromBus(bus &the_bus, std::vector<agents> &the_vector, int Max, int seed, double prob=0.1);

/*Esta función baja gente de las estaciones.
'the_station' es el bus del que se bajan.
'the_vector' es el vector que almacena los agentes que se bajan.
'Max' número máximo de personas que pueden entrar a la estación. Es decir, número máximo de personas que se pueden bajar.
'seed' es la semilla del número aleatorio.
'prob' es el porcentaje de agentes que se bajan.(Por defecto es 0.1).
*/
void GoUpToBus(station &the_station, std::vector<agents> &the_vector, int Max, int seed, double prob=0.1);

/**********************************************************************************************/

int main(void)
{
	//Defino el número de estaciones y buses;
	const int NE = 10;
	const int NB = 2*(NE-1);
	
	//Defino los vectores de buses y estaciones.
	station estaciones[NE];
	bus buses[NB];
	
	//Hago la matriz de adjacencia.
	int matriz[NE*NB];
	for(int i=0; i<(NE*NB); i++){matriz[i]=0;}
	//Aquí defino los enlaces entre las estaciones y los buses
	for(int i=0; i<NE; i++){
		matriz[i*NB + i] = 1;		
		matriz[i*NB + (NB-(i+1))] = 1;		
	}	
	
	imprimir_matriz(matriz,NE,2*(NE-1));
	
	//Aquí le digo a cada bus de cuál estación deja subir gente y de cuál deja bajar
	for(int i=0; i<(NE-1); i++){
		buses[i].station_up = i;
		buses[i].station_down = i+1;
		buses[(NE-1)+i].station_up = (NE-1)-i;
		buses[(NE-1)+i].station_down = (NE-1)-(i+1);
		//std::cout << i << ' ' << i << ' ' << i+1 << std::endl;
		//std::cout << (NE-1)+i << ' ' << (NE-1)-i << ' ' << (NE-1)-(i+1) << std::endl;
	}
	
	//Aquí le asigno la población inicial a las estaciones y los buses
	Crandom bran(0), sran(300), prob(12);
	int the_bus, the_station;
	agents my_agent;
	for(unsigned int i=0; i<NP; i++){
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
	double T=50;
	
	//Creo los arreglos auxiliares para el intercambio de personas	
	std::vector<agents> go_up_to_bus[NB];
	std::vector<agents> go_down_from_bus[NB];
	bus aux_buses[NB];
	
	//Declaro las variables auxiliares que dependen del bus.
	int sta_down, sta_up;
	
	for(int i=0; i<NB; i++){std::cout << buses[i].N() << '\t';}
	std::cout << std::endl;	
	for(int i=0; i<NE; i++){std::cout << estaciones[i].N() << '\t';}
	std::cout << std::endl;
	
	std::cout << NP << std::endl;
	
	//Genero la variable para imprimir los datos.
	std::ofstream fout;	
	
	//Abro el archivo para imprimir los susceptibles.
	fout.open("Datos/susceptibles.csv", std::ofstream::app);
	//Imprimo los datos
	fout << 0;
	for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ns.size();}
	for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ns.size();}
	fout << std::endl;
	fout.close();
			
	//Abro el archivo para imprimir los susceptibles.
	fout.open("Datos/expuestos.csv", std::ofstream::app);
	//Imprimo los datos
	fout << 0;
	for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ne.size();}
	for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ne.size();}
	fout << std::endl;
	fout.close();
			
	//Abro el archivo para imprimir los susceptibles.
	fout.open("Datos/infectados.csv", std::ofstream::app);
	//Imprimo los datos
	fout << 0;
	for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ni.size();}
	for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ni.size();}
	fout << std::endl;
	fout.close();

	//Genero la dinámica 
	for(double t=0; t<T; t++){	
		std::cout << "Time: " << t << '\t';
		//Hago el Gillespie en cada uno de los buses y estaciones
		for(int j=0; j<NB; j++){if(buses[j].Ni.size() != 0 && buses[j].Ns.size() != 0){Gillespie_buses(buses[j],j,t);}}
		for(int j=0; j<NE; j++){Gillespie_estaciones(estaciones[j],j,t);}
		
		//Le doy los valores actuales de cada bus a los arreglos auxiliares.
		for(int j=0; j<NB; j++){aux_buses[j] = buses[j];}
		
		//Hago el intercambio de personas de buses a estaciones.
		for(int j=0; j<NB; j++){
			//Miró a qué estación se baja y a qué estación se sube.			
			sta_down = aux_buses[j].station_down;
			sta_up = aux_buses[j].station_up;			
			
			//Guardo las personas que se bajan del bus. Las quito del bus, sin agregarlas a la estación.
			GoDownFromBus(aux_buses[j],go_down_from_bus[j],estaciones[sta_down].Nmax-estaciones[sta_down].N(),j*NB);			
			
			//Guardo las personas que se suben al bus. Las quito de la estación, sin agregarlas al bus.
			GoUpToBus(estaciones[sta_up],go_up_to_bus[j],aux_buses[j].Nmax-aux_buses[j].N(),sta_up*NB+j);
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
		
		//Hacer el intercambio de pasajeros de un bus a otro.
		for(int j=0; j<NB; j++){buses[(j+1)%NB] = aux_buses[j];}		
		
		std::cout << t << std::endl;
		std::cout << NP << std::endl;
		
		//Abro el archivo para imprimir los susceptibles.
		fout.open("Datos/susceptibles.csv", std::ofstream::app);
		//Imprimo los datos
		fout << t+1;
		for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ns.size();}
		for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ns.size();}
		fout << std::endl;
		fout.close();
		
		//Abro el archivo para imprimir los susceptibles.
		fout.open("Datos/expuestos.csv", std::ofstream::app);
		//Imprimo los datos
		fout << t+1;
		for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ne.size();}
		for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ne.size();}
		fout << std::endl;
		fout.close();
		
		//Abro el archivo para imprimir los susceptibles.
		fout.open("Datos/infectados.csv", std::ofstream::app);
		//Imprimo los datos
		fout << t+1;
		for(int j=0; j<NB; j++){fout << '\t' << buses[j].Ni.size();}
		for(int j=0; j<NE; j++){fout << '\t' << estaciones[j].Ni.size();}
		fout << std::endl;
		fout.close();
	}	
	
	for(int i=0; i<NB; i++){std::cout << buses[i].N() << '\t';}
	std::cout << std::endl;	
	for(int i=0; i<NE; i++){std::cout << estaciones[i].N() << '\t';}
	std::cout << std::endl;	
	
	return 0;	
}

/**********************************************************************************************/

void imprimir_matriz(int *y, int N, int M)
{
	for(int i=0; i<N; i++){
		for(int j=0; j<M; j++){
			std::cout << y[i*M + j] << ' ';			
		}
		std::cout << std::endl;
	}
}

/**********************************************************************************************/

void Gillespie_buses(bus &the_bus, int i, double t, int T)
{
	//Inicio el tiempo
	double aux_t=0;
	
	//Creo los números aleatorios que me dirán el 'dt', el evento que pasará y el agente a escoger que realizará el evento.
	Crandom ran1(145), ran2(324), ran3(897);
	
	//Creo las variables a utilizar del algoritmo.
	double prob, a1, A, dt;	
	
	//Variable del agente que realizará el evento.
	int the_agent;
	
	//Abro el archivo para imprimir los datos
	std::ofstream fout, fout1;	
	
	while(aux_t<T){
		//Defino la probabilidad con la cual se realizará el evento.
		prob = ran2.r();
		
		//Calculo el 'dt' en el que se realizará el evento.
		a1=beta*the_bus.Ns.size()*the_bus.Ni.size();
		A=a1;
		dt = -(1.0/A)*log(ran1.r());
		aux_t += dt;
		
		//Escojo el susceptible que pasará a ser expuesto.
		the_agent = (int)(ran3.r()*the_bus.Ns.size());
		
		//Hago que el agente pase a ser expuesto y deje de ser susceptible si hay algún susceptible
		if(the_bus.Ns.size() > 0){
			//Le digo al agente que pasa a ser expuesto.
			the_bus.Ns[the_agent].susceptible = false;
			the_bus.Ns[the_agent].exposed = true;			
			//Paso el agente al vector de expuestos.
			the_bus.Ne.push_back(the_bus.Ns[the_agent]);
			//Elimino al agente del vector de susceptibles.
			the_bus.Ns.erase( the_bus.Ns.begin() + the_agent);			
		}
	
		//Imprimo los datos
		fout.open("Datos/bus_" + std::to_string(i) + ".csv", std::ofstream::app);
		fout << t + aux_t << '\t' << the_bus.Ns.size() << '\t' << the_bus.Ne.size() << '\t' << the_bus.Ni.size() << std::endl;
		fout.close();
		fout1.open("Datos/bus.csv", std::ofstream::app);		
		fout1 << t + aux_t << '\t' << the_bus.Ns.size() << '\t' << the_bus.Ne.size() << '\t' << the_bus.Ni.size() << std::endl;
		fout1.close();
	}
}

/**********************************************************************************************/

void Gillespie_estaciones(station &the_station, int i, double t, int T)
{
	//Inicio el tiempo
	double aux_t=0;
	
	//Creo los números aleatorios que me dirán el 'dt', el evento que pasará y el agente a escoger que realizará el evento.
	Crandom ran1(65), ran2(960), ran3(35);
	
	//Creo las variables a utilizar del algoritmo.
	double prob, a1, a2, a3, A, dt, S, E, I, N;
	
	//Variable del agente que realizará el evento.
	int the_agent, out_agent;	
	
	//Abro el archivo para imprimir los datos
	std::ofstream fout;
	fout.open("Datos/station_" + std::to_string(i) + ".csv", std::ofstream::app);
	
	while(aux_t<T){
		//Defino la probabilidad con la cual se realizará el evento.
		prob = ran2.r();
		
		//Miro cuántos susceptibles, expuestos e infectados hay.
		S = the_station.Ns.size();
		E = the_station.Ne.size();
		I = the_station.Ni.size();
		N = the_station.N();
		
		//Calculo el 'dt' en el que se realizará el evento.
		a1 = beta*S*I; //Un susceptible se expone.
		a2 = Sa*(the_station.Nmax-N); //Un susceptible entra a la estación.
		a3 = Sa*N; //Un agente sale de la estación.
		A = a1 + a2 + a3;
		dt = -(1.0/A)*log(ran1.r());
		aux_t += dt;
		
		//Se escoge el evento.
		if(prob < a1/A){//Un susceptible se expone.
			//Se escoge el individuo al azar
			the_agent = (int)(ran3.r()*S);
			//Le digo al agente que pasa a ser expuesto.
			the_station.Ns[the_agent].susceptible = false;
			the_station.Ns[the_agent].exposed = true;			
			//Paso el agente al vector de expuestos.
			the_station.Ne.push_back(the_station.Ns[the_agent]);
			//Elimino al agente del vector de susceptibles.
			the_station.Ns.erase( the_station.Ns.begin() + the_agent);			
		}
		else if(prob < (a1+a2)/A){//Ingresa un susceptible.
			//Se crea el agente.
			agents new_agent;
			//Se le indica en dónde está.
			new_agent.location = i;
			//Se le da un número identificador.
			new_agent.number = NP;			
			//Se agraga el id al vector de susceptibles de la estación.
			the_station.Ns.push_back(new_agent);
			//Le sumo 1 a la variable que me dice cuántos agentes hay.
			NP++;
		}
		else{
			//Escojo al azar el agente que sale.
			out_agent = (int)(ran3.r()*N);
			if(out_agent < S){//Se va un susceptible.				
				//Quito el agente de la estación.
				the_station.Ns.erase( the_station.Ns.begin() + out_agent);				
			}
			else if(out_agent < S + I){//Se va el infectado.				
				//Quito el agente de la estación.
				the_station.Ni.erase( the_station.Ni.begin() + 0);
				std::cout << "El infectado se fue" << std::endl;
			}
			else{//Se va un expuesto.				
				//Quito el agente de la estación.
				the_station.Ne.erase( the_station.Ne.begin() + (out_agent-(S+I)) );				
			}			
		}
		
		//Imprimo los datos
		fout << t + aux_t << '\t' << the_station.Ns.size() << '\t' << the_station.Ne.size() << '\t' << the_station.Ni.size() << std::endl;
	}
	fout.close();
}

/**********************************************************************************************/

void GoDownFromBus(bus &the_bus, std::vector<agents> &the_vector, int Max, int seed, double prob)
{
	//Creo el número aleatorio que me dirá qué agente se bajará.
	Crandom ran(seed);	
	
	//Hago la dinámica de bajar la gente de los buses
	for(int i=0; i<the_bus.N(); i++){
		if(the_vector.size() == Max){break;}
		
		if(ran.r() < prob){//Miro aleatoriamente si el agente se baja			
			if(i < the_bus.Ns.size()){//Si es un susceptible, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_bus.Ns[i]);
				//Elimino el agente susceptible del vector. 
				the_bus.Ns.erase( the_bus.Ns.begin() + i);						
			}
			else if(i < the_bus.Ns.size() + the_bus.Ni.size()){//Si es un infectado, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_bus.Ni[0]);
				//Elimino el agente infectado del vector.
				the_bus.Ni.erase( the_bus.Ni.begin() + 0);	
				std::cout << "El infectado se baja del bus" << std::endl;
			}
			else{//Si es un expuesto, lo bajo.	
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_bus.Ne[i-(the_bus.Ns.size() + the_bus.Ni.size())]);
				//Elimino el agente expuesto del vector.
				the_bus.Ne.erase( the_bus.Ne.begin() + i-(the_bus.Ns.size() + the_bus.Ni.size()));
			}			
			//Como se quitó un agente, entonces se debe restar para poder obervarlos todos.
			i--;
		}
	}
}

/**********************************************************************************************/

void GoUpToBus(station &the_station, std::vector<agents> &the_vector, int Max, int seed, double prob)
{
	//Creo el número aleatorio que me dirá qué agente se bajará.
	Crandom ran(seed);	
		
	//Hago la dinámica de bajar la gente de las estaciones
	for(int i=0; i<the_station.N(); i++){
		if(the_vector.size() == Max){break;}
			
		if(ran.r() < prob){//Miro aleatoriamente si el agente se baja			
			if(i < the_station.Ns.size()){//Si es un susceptible, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_station.Ns[i]);
				//Elimino el id del susceptible del vector. 
				the_station.Ns.erase( the_station.Ns.begin() + i);						
			}
			else if(i < the_station.Ns.size() + the_station.Ni.size()){//Si es un infectado, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_station.Ni[0]);
				//Elimino el id del infectado del vector.
				the_station.Ni.erase( the_station.Ni.begin() + 0);
				std::cout << "El infectado se sube al bus" << std::endl;
			}
			else{//Si es un expuesto, lo bajo.
				//Guardo al agente en el vector auxiliar.
				the_vector.push_back(the_station.Ne[i-(the_station.Ns.size() + the_station.Ni.size())]);
				//Elimino el id del expuesto del vector.
				the_station.Ne.erase( the_station.Ne.begin() + i-(the_station.Ns.size() + the_station.Ni.size()));
			}			
			//Como se quitó un agente, entonces se debe restar para poder obervarlos todos.
			i--;
		}
	}
}


