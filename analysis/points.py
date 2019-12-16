# Plotting point trajectories

import numpy as np
import matplotlib.pyplot as plt

figNum = 0
X = np.array([])

def onclick(event):
    global figNum

    m,n = X.shape
    event.canvas.figure.clear()
    event.canvas.figure.gca().plot(X[:,figNum])
    event.canvas.draw()
    figNum += 1

    if figNum >= n:
        plt.close()


def plotPoints( fileName, idxs = -1 ):
    global X

    f = open( fileName, 'r' )

    for line in f:
        positions = line.split(',')
        data = np.zeros((1,len(positions)-1))

        for i in range(1,len(positions)):
            data[0, i-1] = float(positions[i])

        if X.size == 0:
            X = data
        else:
            X = np.append(X, data, axis=0)

    fig = plt.figure()
    fig.canvas.mpl_connect('button_press_event', onclick)
    #plt.plot( X[:,np.random.randint(n)] )

    plt.show()
    f.close()

plotPoints( '../build/points.csv' )