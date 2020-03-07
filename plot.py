import numpy as np
import networkx as nx
from networkx.algorithms import bipartite as bi
import matplotlib.pyplot as plt
import scipy
import pylab

"Esta función me lee el archivo que contenga una matriz y me la retorna ya organizada en una matriz de numpy"
def leer_matrix(archivo, unitaria = False):
    f = open(archivo)
    raw = f.read()
    matrix = [[]]
    numero = ''
    j = 0

    for i in raw:
        if(i == '\n'):
            if(numero != ''):
                matrix[j].append(abs(int(numero)))
            numero = ''
            matrix.append([])
            j += 1
        elif(i == '\t'):
            matrix[j].append(abs(int(numero)))
            numero = ''
        else:
            numero += i

    f.close()
    #matrix.pop()
    matrix.pop()
    
    numpy_matrix = np.matrix(matrix)

    return numpy_matrix


matrix = leer_matrix("Code_CPP/matrix.csv")
adjacency = scipy.sparse.csc_matrix(matrix)

G = bi.from_biadjacency_matrix(adjacency)

infected = leer_matrix("Code_CPP/Datos/infectados.csv")

color = bi.color(G)
pos=nx.spring_layout(G)

(n,l) = infected.shape
#plt.ion()
#plt.show()
#plt.savefig('archivo.gif')

for i in range(n):
    list_color = []
    for m in range(len(color)):
        if(infected.item((i,m+1)) == 1):
            list_color.append('g')
        elif(color[m] == 0):
            list_color.append('b')
        else:
            list_color.append('r')
        
    nx.draw_networkx(G,pos, node_color=list_color, font_size = 11, node_size = 200)
    plt.pause(0.1)
    plt.clf()
    #plt.title("Gráfica")
    #plt.savefig('Imagen/' + legend + '.pdf')
    #plt.show(block=False)
    #plt.clf()

