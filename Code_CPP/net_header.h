#ifndef NET_HEADER_H_
#define NET_HEADER_H_

#include <vector>

//Genero el alias de la matriz
typedef std::vector<std::vector<int> > network;

/* Esta función me genera una red aleatoria Erdös-Renyi
 * 'y' es la red.
 * 'N' es el tamaño de la red.
 * 'prob' es la probabilidad con que se conecta.
 * 'seed' es la semilla para el número aleatorio.
 */
void Erdos_renyi(network &y, int N, double prob, int seed=0);

/* Esta función me hace una Ego red de N nodos
 * 'y' es la red.
 * 'N' es el número de nodos. 
 */
void Ego_red(network &y, int N);

#endif