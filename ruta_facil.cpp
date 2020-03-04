#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "agents.h"
#include "Random64.h"
#include "bus_station.h"

/**********************************************************************************************/
//Defino las variables globales de la epidemia
const double beta = 0.5;
const double Sa = 0.1;

//Defino la variable global del número de agentes.
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
'people' es el vector que contiene a todos los agentes.
'i' es el número del bus.
't' es el tiempo absoluto.
'T' es el tiempo que va a durar el algoritmo.
*/
void Gillespie_buses(bus &the_bus, std::vector<agents> &people, int i, double t, int T=1);

/*Esta función hace el gillespie en las estaciones.
'the_station' es la estación que va a tener la dinámica.
'people' es el vector que contiene a todos los agentes.
'i' es el número del bus.
't' es el tiempo absoluto.
'T' es el tiempo que va a durar el algoritmo.
*/
void Gillespie_estaciones(station &the_station, std::vector<agents> &people, int i, double t, int T=1);

/*Esta función baja gente de los buses.
'the_bus' es el bus del que se bajan.
'people' es el vector que contiene a todos los agentes.
'the_vector' es el vector que almacena los agentes que se bajan.
'Max' número máximo de personas que pueden entrar a la estación. Es decir, número máximo de personas que se pueden bajar.
'seed' es la semilla del número aleatorio.
'prob' es el porcentaje de agentes que se bajan.(Por defecto es 0.1).
*/
void GoDownFromBus(bus &the_bus, std::vector<agents> &people, std::vector<agents> &the_vector, int Max, int seed, double prob=0.1);

/*Esta función baja gente de las estaciones.
'the_station' es el bus del que se bajan.
'people' es el vector que contiene a todos los agentes.
'the_vector' es el vector que almacena los agentes que se bajan.
'Max' número máximo de personas que pueden entrar a la estación. Es decir, número máximo de personas que se pueden bajar.
'seed' es la semilla del número aleatorio.
'prob' es el porcentaje de agentes que se bajan.(Por defecto es 0.1).
*/
void GoUpToBus(station &the_station, std::vector<agents> &people, std::vector<agents> &the_vector, int Max, int seed, double prob=0.1);

/**********************************************************************************************/

int main(void)
{
	//Defino el número de estaciones y buses;
	const int NE = 10;
	const int NB = 2*(NE-1);
	
	//Defino los vectores de buses y estaciones.
	station estaciones[NE];
	bus buses[NB];
	
	//Creo los agentes
	std::vector<agents> people;
	people.resize(NP);
	
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
	Crandom bran(0), sran(300), prob(123);
	int the_bus, the_station;
	for(unsigned int i=0; i<people.size(); i++){
		people[i].number = i;
		if(prob.r() < 0.3){
			people[i].in_bus = true;
			people[i].in_station = false;
			the_bus = (int)(bran.r()*NB);
			people[i].location = the_bus;
			buses[the_bus].Ns.push_back(i);
		}
		else{
			the_station = (int)(sran.r()*NE);
			people[i].location = the_station;
			estaciones[the_station].Ns.push_back(i);
		}		
	}
	
	//Escojo la persona infectada. La agrego al vector Ni y lo quito del Ns.
	int infected = (int)(prob.r()*NP);
	people[infected].susceptible = false;
	people[infected].infected = true;
	int ilocation = people[infected].location;
	if(people[infected].in_bus){
		buses[ilocation].Ni.push_back(infected);
		erase_susceptible(buses,infected,ilocation);
		std::cout << buses[ilocation].Ni.size() << '\t' << buses[ilocation].Ns.size() << std::endl;
	}
	else{
		estaciones[ilocation].Ni.push_back(infected);
		erase_susceptible(estaciones,infected,ilocation);
		std::cout << estaciones[ilocation].Ni.size() << '\t' << estaciones[ilocation].Ns.size() << std::endl;
	}
	
	//Digo cuántos pasos de tiempo va a dar la simulación
	double T=10;
	
	//Creo los arreglos auxiliares para el intercambio de personas	
	std::vector<agents> go_up_to_bus[NB];
	std::vector<agents> go_down_from_bus[NB];		
	
	//Declaro las variables auxiliares que dependen del bus.
	int sta_down, sta_up;
	
	for(int i=0; i<NB; i++){std::cout << buses[i].N() << '\t';}
	std::cout << std::endl;	
	for(int i=0; i<NE; i++){std::cout << estaciones[i].N() << '\t';}
	std::cout << std::endl;	
	
	std::cout << people.size() << std::endl;

	//Genero la dinámica 
	for(double t=0; t<T; t++){	
		std::cout << "Time: " << t << '\t';
		//Hago el Gillespie en cada uno de los buses y estaciones
		for(int j=0; j<NB; j++){if(buses[j].Ni.size() != 0 && buses[j].Ns.size() != 0){Gillespie_buses(buses[j],people,j,t);}}
		for(int j=0; j<NE; j++){Gillespie_estaciones(estaciones[j],people,j,t);}		
		
		//Hago el intercambio de personas de buses a estaciones.
		for(int j=0; j<NB; j++){
			//Miró a qué estación se baja y a qué estación se sube.			
			sta_down = buses[j].station_down;
			sta_up = buses[j].station_up;			
			
			//Guardo las personas que se bajan del bus. Las quito del bus, sin agregarlas a la estación.
			GoDownFromBus(buses[j],people,go_down_from_bus[j],estaciones[sta_down].Nmax-estaciones[sta_down].N(),j*NB);			
			
			//Guardo las personas que se suben al bus. Las quito de la estación, sin agregarlas al bus.
			GoUpToBus(estaciones[sta_up],people,go_up_to_bus[j],buses[j].Nmax-buses[j].N(),sta_up*NB+j);
		}
		
		std::cout << t << '\t';
		//Agrego las personas que bajé del bus a la estación.
		for(int j=0; j<NB; j++){
			sta_down = buses[j].station_down;
			for(int k=0; k<go_down_from_bus[j].size(); k++){
				estaciones[sta_down] += go_down_from_bus[j][k];
			}
			go_down_from_bus[j].clear();
		}
		
		std::cout << t << '\t';
		//Agrego las personas que bajé de la estación al bus.
		for(int j=0; j<NB; j++){
			sta_up = buses[j].station_up;
			for(int k=0; k<go_up_to_bus[j].size(); k++){
				buses[j] += go_up_to_bus[j][k];
			}
			go_up_to_bus[j].clear();
		}
		
		std::cout << t << std::endl;
		std::cout << people.size() << std::endl;
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

void Gillespie_buses(bus &the_bus, std::vector<agents> &people, int i, double t, int T)
{
	//Inicio el tiempo
	double aux_t=0;
	
	//Creo los números aleatorios que me dirán el 'dt', el evento que pasará y el agente a escoger que realizará el evento.
	Crandom ran1(145), ran2(324), ran3(897);
	
	//Creo las variables a utilizar del algoritmo.
	double prob, a1, A, dt;
	
	//Variable del agente que realizará el evento.
	int location_agt, number_agt;
	
	//Abro el archivo para imprimir los datos
	std::ofstream fout;
	fout.open("Datos/bus_" + std::to_string(i) + ".csv", std::ofstream::app);
	
	while(aux_t<T){
		//Defino la probabilidad con la cual se realizará el evento.
		prob = ran2.r();
		
		//Calculo el 'dt' en el que se realizará el evento.
		a1=beta*the_bus.Ns.size()*the_bus.Ni.size();
		A=a1;
		dt = -(1.0/A)*log(ran1.r());
		aux_t += dt;
		
		//Escojo el susceptible que pasará a ser expuesto.
		location_agt = (int)(ran3.r()*the_bus.Ns.size());
		
		//Hago que el agente pase a ser expuesto y deje de ser susceptible si hay algún susceptible
		if(the_bus.Ns.size() > 0){
			number_agt = the_bus.Ns[location_agt];
			the_bus.Ne.push_back(number_agt);
			the_bus.Ns.erase( the_bus.Ns.begin() + location_agt);
			people[number_agt].susceptible = false;
			people[number_agt].exposed = true;
		}
		
		//Imprimo los datos
		fout << t + aux_t << '\t' << the_bus.Ns.size() << '\t' << the_bus.Ne.size() << '\t' << the_bus.Ni.size() << std::endl;
	}
	fout.close();
}

/**********************************************************************************************/

void Gillespie_estaciones(station &the_station, std::vector<agents> &people, int i, double t, int T)
{
	//Inicio el tiempo
	double aux_t=0;
	
	//Creo los números aleatorios que me dirán el 'dt', el evento que pasará y el agente a escoger que realizará el evento.
	Crandom ran1(65), ran2(960), ran3(35);
	
	//Creo las variables a utilizar del algoritmo.
	double prob, a1, a2, a3, A, dt, S, E, I, N;
	
	//Variable del agente que realizará el evento.
	int location_agt, number_agt, out_agent;
	
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
			location_agt = (int)(ran3.r()*S);
			number_agt = the_station.Ns[location_agt];
			the_station.Ne.push_back(number_agt);
			the_station.Ns.erase( the_station.Ns.begin() + location_agt);
			people[number_agt].susceptible = false;
			people[number_agt].exposed = true;
		}
		else if(prob < (a1+a2)/A){//Ingresa un susceptible.
			NP++;
			agents new_agent;
			new_agent.location = i;
			new_agent.number = NP;
			people.push_back(new_agent);
			the_station.Ns.push_back(people.size()-1);
		}
		else{
			out_agent = (int)(ran3.r()*N);
			if(out_agent < S){//Se va un susceptible.
				number_agt = the_station.Ns[out_agent];
				the_station.Ns.erase( the_station.Ns.begin() + out_agent);
				people[number_agt].in_station = false;
				people[number_agt].in_bus = false;
			}
			else if(out_agent < S + I){//Se va el infectado.
				number_agt = the_station.Ni[0];
				the_station.Ni.erase( the_station.Ni.begin() + 0);
				people[number_agt].in_station = false;
				people[number_agt].in_bus = false;
			}
			else{//Se va un expuesto.
				number_agt = the_station.Ne[out_agent-(S+I)];
				the_station.Ne.erase( the_station.Ne.begin() + (out_agent-(S+I)) );
				people[number_agt].in_station = false;
				people[number_agt].in_bus = false;
			}
		}
		
		//Imprimo los datos
		fout << t + aux_t << '\t' << the_station.Ns.size() << '\t' << the_station.Ne.size() << '\t' << the_station.Ni.size() << std::endl;
	}
	fout.close();
}

/**********************************************************************************************/

void GoDownFromBus(bus &the_bus, std::vector<agents> &people, std::vector<agents> &the_vector, int Max, int seed, double prob)
{
	//Creo el número aleatorio que me dirá qué agente se bajará.
	Crandom ran(seed);
	
	//Declaro la varible que se le asignara al agente.
	int num_agent;
	
	//Hago la dinámica de bajar la gente de los buses
	for(int i=0; i<the_bus.N(); i++){
		if(the_vector.size() == Max){break;}
		
		if(ran.r() < prob){//Miro aleatoriamente si el agente se baja			
			if(i < the_bus.Ns.size()){//Si es un susceptible, lo bajo.
				num_agent = the_bus.Ns[i];
				the_bus.Ns.erase( the_bus.Ns.begin() + i);
				people[num_agent].in_bus = false;
				people[num_agent].in_station = true;				
			}
			else if(i < the_bus.Ns.size() + the_bus.Ni.size()){//Si es un infectado, lo bajo.
				num_agent = the_bus.Ni[0];
				the_bus.Ni.erase( the_bus.Ni.begin() + 0);
				people[num_agent].in_bus = false;
				people[num_agent].in_station = true;
			}
			else{//Si es un expuesto, lo bajo.
				num_agent = the_bus.Ne[i-(the_bus.Ns.size() + the_bus.Ni.size())];
				the_bus.Ne.erase( the_bus.Ne.begin() + i-(the_bus.Ns.size() + the_bus.Ni.size()));
				people[num_agent].in_bus = false;
				people[num_agent].in_station = true;				
			}
			the_vector.push_back(people[num_agent]);			
		}
	}
}

/**********************************************************************************************/

void GoUpToBus(station &the_station, std::vector<agents> &people, std::vector<agents> &the_vector, int Max, int seed, double prob)
{
	//Creo el número aleatorio que me dirá qué agente se bajará.
	Crandom ran(seed);
	
	//Declaro la varible que se le asignara al agente.
	int num_agent;
		
	//Hago la dinámica de bajar la gente de las estaciones.
	for(int i=0; i<the_station.N(); i++){
		if(the_vector.size() == Max){break;}
			
		if(ran.r() < prob){//Miro aleatoriamente si el agente se baja			
			if(i < the_station.Ns.size()){//Si es un susceptible, lo bajo.
				num_agent = the_station.Ns[i];
				the_station.Ns.erase( the_station.Ns.begin() + i);
				people[num_agent].in_bus = true;
				people[num_agent].in_station = false;				
			}
			else if(i < the_station.Ns.size() + the_station.Ni.size()){//Si es un infectado, lo bajo.
				num_agent = the_station.Ni[0];
				the_station.Ni.erase( the_station.Ni.begin() + 0);
				people[num_agent].in_bus = true;
				people[num_agent].in_station = false;
			}
			else{//Si es un expuesto, lo bajo.
				num_agent = the_station.Ne[i-(the_station.Ns.size() + the_station.Ni.size())];
				the_station.Ne.erase( the_station.Ne.begin() + i-(the_station.Ns.size() + the_station.Ni.size()));
				people[num_agent].in_bus = true;
				people[num_agent].in_station = false;				
			}
			the_vector.push_back(people[num_agent]);			
		}
	}
}


