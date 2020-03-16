#include "net_header.h"
#include "Random64.h"

void Erdos_renyi(network &y, int N, double prob, int seed)
{
	//Declaro la variable aleatoria que me genera la red
	Crandom ran(seed);
	
	//Creo la matriz
	y.resize(N);
	for(int i=0; i<N; i++){y[i].resize(N);}
	
	//Hago la matriz de adyacencia
	for(int i=0; i<N; i++){		
		for(int j=0; j<N; j++){
			if(ran.r() < prob){y[i][j]=1;	y[j][i]=1;}//Si el número aleatorio es menor a la probabilidad, entonces hay conexión.
			else{y[i][j]=0;		y[j][i]=0;}//Si no, pues no.
		}
	}
}


void Ego_red(network &y, int N)
{
	//Creo la matriz
	y.resize(N);
	for(int i=0; i<N; i++){y[i].resize(N);}
	
	//Hago la matriz de adyacencia
	y[0][0] = 0;
	for(int i=1; i<N; i++){y[0][i]=1;	y[i][0]=1;}
	
	for(int i=1; i<N; i++){
		for(int j=i; j<N; j++){
			y[i][j]=0;		y[j][i]=0;
		}
	}
}
