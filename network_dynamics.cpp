#include "network_dynamics.h"

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

void Gillespie_estaciones(station &the_station, int i, double t, int id_NP, int count_NP, int T)
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
			new_agent.number = id_NP;			
			//Se agraga el id al vector de susceptibles de la estación.
			the_station.Ns.push_back(new_agent);
			//Le sumo 1 a la variable que me identifica los agentes y también a la que me dice cuántos agentes hay.
			id_NP++;
			count_NP++;
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
			//Le resto 1 al contador de agentes.
			count_NP--;
		}
		
		//Imprimo los datos
		fout << t + aux_t << '\t' << the_station.Ns.size() << '\t' << the_station.Ne.size() << '\t' << the_station.Ni.size() << std::endl;
	}
	fout.close();
}

/**********************************************************************************************/
