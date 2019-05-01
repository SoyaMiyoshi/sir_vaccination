import networkx as nx
from sys import argv
from random import getrandbits
import numpy as np
import subprocess

nrep = 10 # how many networks to average over
#betas = [0.5,1.0,2.0] # beta values to test
betas = [1.0]
cr = 0.8

fname = '.tmp_nwkfile'

def payoffs(probs):
	pass


if __name__ == "__main__":
	
	if len(argv) != 3:
		print ('usage: python sir.py [number of nodes] [number of links]')
		exit()

	n = int(argv[1])
	m = int(argv[2])

	for beta in betas:
		probs = np.array([0 for i in range(n)])
		for irep in range(nrep):
			G = nx.gnm_random_graph(n, m)  # generate network
			nx.write_edgelist(G, fname, data=False)
			out = subprocess.getoutput('./sir' + ' ' + fname + ' ' + str(beta) + str(getrandbits(64)))
			for i in out.split("\n"):
				print(i.split(' '))
			#temp = np.array([float(i.strip().split(' ')[1]) for i in out.split("\n")])
			#probs = (probs + temp)/2
