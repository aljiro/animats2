# Plotting point trajectories

import numpy as np
import matplotlib.pyplot as plt


def plotPoints( fileName, idxs = -1 ):
    f = open( fileName, 'r' )
    X = np.array([])

    for line in f:
        positions = line.split(',')
        data = np.zeros((1,len(positions)-1))

        for i in range(1,len(positions)):
            data[0, i-1] = float(positions[i])

        if X.size == 0:
            X = data
        else:
            X = np.append(X, data, axis=0)

    m,n = X.shape

    for i in range(n):
        plt.plot( X[:,i] + np.ones(m)*i)
    #plt.plot( X[:,np.random.randint(n)] )

    plt.show()
    f.close()

plotPoints( '../build/points.csv' )