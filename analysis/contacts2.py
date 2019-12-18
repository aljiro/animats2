# Plotting point trajectories

import numpy as np
import matplotlib.pyplot as plt

figNum = 0
X = np.array([])


def plotContacts( fileName, idxs = -1 ):
    global X

    f = open( fileName, 'r' )

    for line in f:
        data = float(line)        
        X = np.append(X, data)

    fig = plt.figure()
    
    plt.plot( X )

    plt.show()
    f.close()

plotContacts( '../build/contacts.csv' )