import networkx as nx
from sys import argv
from random import getrandbits
import numpy as np
import subprocess
import csv
import pandas as pd

nrep = 10 # how many networks to average over
#betas = [0.5,1.0,2.0] # beta values to test
betas = [1.0]
cr = 0.8

fname = '.tmp_nwkfile'

def payoffs(probs):
	pass


if __name__ == "__main__":

	"""
	"usage: ./sir 1[nwk file] 2[beta] 3[coverage] 4[efficacy] "
	"5[vac_cost] 6[rationality] 7[conformity] 8[threshold_fraction] <seed>\n");
	"""

	out = subprocess.getoutput("./sir nwk/iceland.lnk 2 0.8 2 0.4 0.1 0.2 0.9")

	with open("example.csv",'w') as csvfile:
		writer = csv.writer(csvfile)
		writer.writerow(["time", "avg outbreak size", "avg time to extinction"])
		time = 0
		for i in out.split("\n"):
			writer.writerow([time] + i.split(' '))
			time += 1
		csvfile.close()



	"""
	
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
	"""
