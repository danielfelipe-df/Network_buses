#include "network_dynamics.h"

/**********************************************************************************************/

void imprimir_matriz(int *y, int N, int M)
{
	std::ofstream fout("matrix.csv");
	for(int i=0; i<N; i++){
		for(int j=0; j<M; j++){
			fout << y[i*M + j] << '\t';			
		}
		fout << std::endl;
	}
	fout.close();
}

/**********************************************************************************************/

void Gillespie_buses(bus &the_bus, std::vector<agents> &aux, int &exposed, int i, double t, 
										 double seed1, double seed2, double seed3,int T)
{
	//Inicio el tiempo
	double aux_t=0;
	
	//Creo los números aleatorios que me dirán el 'dt', el evento que pasará y el agente a escoger que realizará el evento.
	Crandom ran1((int)(seed1*MY_MAX)), ran2((int)(seed2*MY_MAX)), ran3((int)(seed3*MY_MAX));
	
	//Creo las variables a utilizar del algoritmo.
	double prob, a1, a2, A, dt;	
	
	//Variable del agente que realizará el evento.
	int the_agent;
	
	//Abro el archivo para imprimir los datos
	std::ofstream fout, fout1;	
	
	//Creo el iterador para encontrar el agente.
	std::vector<agents>::iterator it;
	
	while(aux_t<T){
		//Defino la probabilidad con la cual se realizará el evento.
		prob = ran2.r();
		
		//Calculo el 'dt' en el que se realizará el evento.
		a1 = beta1*the_bus.Ns1.size()*the_bus.Ni.size();
		a2 = beta2*the_bus.Ns2.size()*the_bus.Ni.size();
		A = a1 + a2;
		dt = -(1.0/A)*log(ran1.r());
		aux_t += dt;
		if(aux_t > T){break;}
		
		//Hago que el agente pase a ser expuesto y deje de ser susceptible si hay algún susceptible.
		if(the_bus.NS() > 0){
			
			//Miro si el susceptible 1 se puede contaminar
			if(prob < a1/A){
				for(int i=0; i<aux.size(); i++){
					if((it = std::find(the_bus.Ns1.begin(),the_bus.Ns1.end(),aux[i])) != the_bus.Ns1.end()){
						//Hallo el item del agente.
						the_agent = std::distance(the_bus.Ns1.begin(),it);
						//Le digo al agente que pasa a ser expuesto.
						the_bus.Ns1[the_agent].susceptible1 = false;
						the_bus.Ns1[the_agent].exposed = true;			
						//Paso el agente al vector de expuestos.
						the_bus.Ne.push_back(the_bus.Ns1[the_agent]);
						//Elimino al agente del vector de susceptibles.
						the_bus.Ns1.erase( the_bus.Ns1.begin() + the_agent);
						//Le digo al agente que pasa a ser expuesto.
						aux[i].susceptible1 = false;
						aux[i].exposed = true;
						//Sumo uno al número de expuestos nuevos.
						exposed++;
						/*
						//Imprimo únicamente los expuestos
						fout.open("Datos/exposed.csv", std::ofstream::app);
						fout << t + aux_t << '\t' << exposed << std::endl;
						fout.close();
						 */
						//Termino el ciclo
						break;
					}				
				}
			}
			//Miro si el susceptible 2 se puede contaminar
			else{
				for(int i=0; i<aux.size(); i++){
					if((it = std::find(the_bus.Ns2.begin(),the_bus.Ns2.end(),aux[i])) != the_bus.Ns2.end()){
						//Hallo el item del agente.
						the_agent = std::distance(the_bus.Ns2.begin(),it);
						//Le digo al agente que pasa a ser expuesto.
						the_bus.Ns2[the_agent].susceptible2 = false;
						the_bus.Ns2[the_agent].exposed = true;			
						//Paso el agente al vector de expuestos.
						the_bus.Ne.push_back(the_bus.Ns2[the_agent]);
						//Elimino al agente del vector de susceptibles.
						the_bus.Ns2.erase( the_bus.Ns2.begin() + the_agent);
						//Le digo al agente que pasa a ser expuesto.
						aux[i].susceptible2 = false;
						aux[i].exposed = true;
						//Sumo uno al número de expuestos nuevos.
						exposed++;
						/*
						//Imprimo únicamente los expuestos
						fout.open("Datos/exposed.csv", std::ofstream::app);
						fout << t + aux_t << '\t' << exposed << std::endl;
						fout.close();
						*/
						//Termino el ciclo
						break;
					}				
				}
			}
		}
	
		/*
		//Imprimo los datos
		fout.open("Datos/bus_" + std::to_string(i) + ".csv", std::ofstream::app);
		fout << t + aux_t << '\t' << the_bus.Ns.size() << '\t' << the_bus.Ne.size() << '\t' << the_bus.Ni.size() << std::endl;
		fout.close();
		fout1.open("Datos/bus.csv", std::ofstream::app);		
		fout1 << t + aux_t << '\t' << the_bus.Ns.size() << '\t' << the_bus.Ne.size() << '\t' << the_bus.Ni.size() << std::endl;
		fout1.close();
		*/
	}
}

/**********************************************************************************************/

bool Gillespie_estaciones(station &the_station, std::vector<agents> &aux, int &exposed, int i, double t,
													int id_NP, int count_NP, double seed1, double seed2, double seed3,int T)
{
	//Inicio el tiempo
	double aux_t=0;
	
	//Creo los números aleatorios que me dirán el 'dt', el evento que pasará y el agente a escoger que realizará el evento.
	Crandom ran1((int)(seed1*MY_MAX)), ran2((int)(seed2*MY_MAX)), ran3((int)(seed3*MY_MAX));
	
	//Creo las variables a utilizar del algoritmo.
	double prob, a1, a2, a3, a4, a5, a6, a7, A, dt, S1, S2, E, I, N;
	
	//Variable del agente que realizará el evento.
	int the_agent, out_agent, agent_mat;	
	
	//Abro el archivo para imprimir los datos
	std::ofstream fout;	
	
	//Creo el iterador 
	std::vector<agents>::iterator it;
	
	while(aux_t<T){
		//Defino la probabilidad con la cual se realizará el evento.
		prob = ran2.r();
		
		//Miro cuántos susceptibles, expuestos e infectados hay.
		S1 = the_station.Ns1.size();
		S2 = the_station.Ns2.size();
		E = the_station.Ne.size();
		I = the_station.Ni.size();
		N = the_station.N();
		
		//Calculo el 'dt' en el que se realizará el evento.
		a1 = beta1*S1*I; //Un susceptible 1 se expone.
		a2 = beta2*S2*I; //Un susceptible 2 se expone.
		a3 = 2*Sa*(the_station.Nmax-N); //Un susceptible entra a la estación.
		a4 = Sa*S1; //Un susceptible 1 sale de la estación.
		a5 = Sa*S2; //Un susceptible 2 sale de la estación.
		a6 = Sa*I*0; //El infectado sale de la estación.
		a7 = Sa*E; //Un expuesto sale de la estación.
		A = a1 + a2 + a3 + a4 + a5 + a6 + a7;
		dt = -(1.0/A)*log(ran1.r());
		aux_t += dt;
		
		if(aux_t > T){break;}
		
		//Se escoge el evento.
		if(prob < a1/A){//Un susceptible 1 se expone.			
			for(int i=0; i<aux.size(); i++){
				if((it = std::find(the_station.Ns1.begin(),the_station.Ns1.end(),aux[i])) != the_station.Ns1.end()){
					//Hallo el item del agente.
					the_agent = std::distance(the_station.Ns1.begin(),it);
					//Le digo al agente que pasa a ser expuesto.
					the_station.Ns1[the_agent].susceptible1 = false;
					the_station.Ns1[the_agent].exposed = true;			
					//Paso el agente al vector de expuestos.
					the_station.Ne.push_back(the_station.Ns1[the_agent]);
					//Elimino al agente del vector de susceptibles.
					the_station.Ns1.erase( the_station.Ns1.begin() + the_agent);
					//Le digo al agente que pasa a ser expuesto.
					aux[i].susceptible1 = false;
					aux[i].exposed = true;
					//Sumo uno al número de expuestos infectados.
					exposed++;
					/*
					//Imprimo únicamente los expuestos
					fout.open("Datos/exposed.csv", std::ofstream::app);
					fout << t + aux_t << '\t' << exposed << std::endl;
					fout.close();
					*/
					//Termino el ciclo
					break;
				}			
			}			
		}
		else if(prob < (a1+a2)/A){//Un susceptible 2 se expone.			
			for(int i=0; i<aux.size(); i++){
				if((it = std::find(the_station.Ns2.begin(),the_station.Ns2.end(),aux[i])) != the_station.Ns2.end()){
					//Hallo el item del agente.
					the_agent = std::distance(the_station.Ns2.begin(),it);
					//Le digo al agente que pasa a ser expuesto.
					the_station.Ns2[the_agent].susceptible2 = false;
					the_station.Ns2[the_agent].exposed = true;			
					//Paso el agente al vector de expuestos.
					the_station.Ne.push_back(the_station.Ns2[the_agent]);
					//Elimino al agente del vector de susceptibles.
					the_station.Ns2.erase( the_station.Ns2.begin() + the_agent);
					//Le digo al agente que pasa a ser expuesto.
					aux[i].susceptible2 = false;
					aux[i].exposed = true;
					//Sumo uno al número de expuestos infectados.
					exposed++;
					/*
					//Imprimo únicamente los expuestos
					fout.open("Datos/exposed.csv", std::ofstream::app);
					fout << t + aux_t << '\t' << exposed << std::endl;
					fout.close();
					*/
					//Termino el ciclo
					break;
				}			
			}			
		}
		else if(prob < (a1+a2+a3)/A){//Ingresa un susceptible.			
			//Se crea el agente.
			agents new_agent(ran2.r());
			//Se le indica en dónde está.
			new_agent.location = i;
			//Se le da un número identificador.
			new_agent.number = id_NP;			
			//Se agraga el id al vector de susceptibles de la estación.
			if(new_agent.susceptible1){the_station.Ns1.push_back(new_agent);}
			else{the_station.Ns2.push_back(new_agent);}
			//Le sumo 1 a la variable que me identifica los agentes y también a la que me dice cuántos agentes hay.
			id_NP++;
			count_NP++;
			//Si el número aleatorio es menor a 0.05, entonces el nuevo susceptible va a estar conectado al infectado.
			if(ran3.r() < 0.05){aux.push_back(new_agent);}				
		}
		else if(prob < (a1+a2+a3+a4)/A){			
			//Escojo al azar el susceptible 1 que sale.
			the_agent = (int)(ran3.r()*S1);
			//Reviso si el agente está conectado con el infectado.			
			if((it = std::find(aux.begin(), aux.end(), the_station.Ns1[the_agent])) != aux.end()){				
				//Quito una fila y columna de la matriz.
				agent_mat = std::distance(aux.begin(),it);								
				//Lo elimino del vector auxiliar.
				aux.erase(it);
			}			
			//Quito el agente de la estación.
			the_station.Ns1.erase( the_station.Ns1.begin() + the_agent);
			//Le resto 1 al contador de agentes.
			count_NP--;
		}
		else if(prob < (a1+a2+a3+a4+a5)/A){			
			//Escojo al azar el susceptible 2 que sale.
			the_agent = (int)(ran3.r()*S2);
			//Reviso si el agente está conectado con el infectado.			
			if((it = std::find(aux.begin(), aux.end(), the_station.Ns2[the_agent])) != aux.end()){				
				//Quito una fila y columna de la matriz.
				agent_mat = std::distance(aux.begin(),it);								
				//Lo elimino del vector auxiliar.
				aux.erase(it);
			}			
			//Quito el agente de la estación.
			the_station.Ns2.erase( the_station.Ns2.begin() + the_agent);
			//Le resto 1 al contador de agentes.
			count_NP--;
		}
		else if(prob < (a1+a2+a3+a4+a5+a6)/A){//Se va el infectado.			
			//Quito el agente de la estación.
			the_station.Ni.erase( the_station.Ni.begin() + 0);
			//Borro el vector auxiliar.
			std::cout << "El infectado se fue" << '\t';
			aux.clear();			
			std::cout << "El infectado se fue" << std::endl;
			return false;
			//Le resto 1 al contador de agentes.
			count_NP--;
		}
		else{//Se va un expuesto.			
			//Escojo al azar el susceptible que sale.
			the_agent = (int)(ran3.r()*E);
			//Reviso si el agente está conectado con el infectado.			
			if((it = std::find(aux.begin(), aux.end(), the_station.Ne[the_agent])) != aux.end()){				
				//Quito una fila y columna de la matriz.
				agent_mat = std::distance(aux.begin(),it);								
				//Lo elimino del vector auxiliar.
				aux.erase(it);
			}			
			//Quito el agente de la estación.
			the_station.Ne.erase( the_station.Ne.begin() + the_agent );
			//Le resto 1 al contador de agentes.
			count_NP--;			
		}
		 
		/*
		//Imprimo los datos
		fout.open("Datos/station_" + std::to_string(i) + ".csv", std::ofstream::app);
		fout << t + aux_t << '\t' << the_station.Ns.size() << '\t' << the_station.Ne.size() << '\t' << the_station.Ni.size() << std::endl;
		fout.close();
		*/
	}
	return true;
}

/**********************************************************************************************/
