import networkx as nx
import subprocess
import csv
import os
from random import *

nrep = 10  # how many networks to average over
nwk_size = 1000
num_edges = 2 * nwk_size
seasons = 100  # defined in C file

fname = ".tmp_nwkfile"


# "usage: ./sir 1[nwk file] 2[beta] 3[initial coverage] \n"
# "4[cost of vaccination] 5[fraction of conformist] 6[fraction of zealot] <seed>\n"

def run_simulation(beta, coverage, vac_cost, frac_conf, frac_zealot, variable):
    outbreak_avg = [0 for i in range(seasons)]
    time_to_extn_avg = [0 for i in range(seasons)]
    coverage_avg = [0 for i in range(seasons)]
    time = [i for i in range(seasons)]

    for rep in range(nrep):
        G = nx.gnm_random_graph(nwk_size, num_edges)  # generate network
        nx.write_edgelist(G, fname, data=False)

        command = (
                "./sir .tmp_nwkfile"
                + " "
                + str(beta)
                + " "
                + str(coverage)
                + " "
                + str(vac_cost)
                + " "
                + str(frac_conf)
                + " "
                + str(frac_zealot)
                + " "
                + variable
                + " "
                + str(getrandbits(64))
        )

        # out = subprocess.getoutput("./sir .tmp_nwkfile 2 0.8 2 0.4 0.1 0.2 0.9")
        out = subprocess.getoutput(command)
        print(out)

        print("----------------------")

        i = 0
        for j in out.split("\n"):
            line = j.split(" ")
            outbreak_avg[i] += float(line[0])
            time_to_extn_avg[i] += float(line[1])
            coverage_avg[i] += float(line[2])
            i += 1

    outbreak_avg = [float(i) / nrep for i in outbreak_avg]
    time_to_extn_avg = [float(i) / nrep for i in time_to_extn_avg]
    coverage_avg = [float(i) / nrep for i in coverage_avg]

    dir_name = "output/" + "nwksize=" + str(nwk_size) + "_nedge=" + str(num_edges/nwk_size) + "/"

    try:
        os.mkdir(dir_name)
        print("Directory ", dir_name, " Created ")
    except FileExistsError:
        print("Directory ", dir_name, " already exists")

    directory_make = dir_name

    directory_make += "b=" + str(beta) + "/"
    try:
        os.mkdir(directory_make)
        print("Directory ", directory_make, " Created ")
    except FileExistsError:
        print("Directory ", dir_name, " already exists")

    directory_make += "c=" + str(coverage) + "/"
    try:
        os.mkdir(directory_make)
        print("Directory ", directory_make, " Created ")
    except FileExistsError:
        print("Directory ", dir_name, " already exists")

    directory_make += "v=" + str(vac_cost) + "/"
    try:
        os.mkdir(directory_make)
        print("Directory ", directory_make, " Created ")
    except FileExistsError:
        print("Directory ", dir_name, " already exists")

    directory_make += "cf=" + str(frac_conf) + "/"
    try:
        os.mkdir(directory_make)
        print("Directory ", directory_make, " Created ")
    except FileExistsError:
        print("Directory ", dir_name, " already exists")

    directory_make += "zf=" + str(frac_zealot) + "/"
    try:
        os.mkdir(directory_make)
        print("Directory ", directory_make, " Created ")
    except FileExistsError:
        print("Directory ", dir_name, " already exists")


    file_name = directory_make + "data.csv"

    try:
        with open(file_name, "w") as csvfile:
            print("writing", file_name)
            writer = csv.writer(csvfile)
            writer.writerow(
                ["time", "avg outbreak size", "avg time to extinction", "coverage"]
            )
            rows = zip(time, outbreak_avg, time_to_extn_avg, coverage_avg)
            i = 0
            for row in rows:
                # if i != 0:
                #    writer.writerow(row)
                writer.writerow(row)
                i += 1
            csvfile.close()

    except FileNotFoundError:
        print("Not found!")
        return 1


betas = [1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.1, 2.2, 2.3]  # beta values to test
coverages = [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95]
vac_costs = [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95]  # btwn 0 to 1 (cost of falling illness is set to 1)
frac_confs = [0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95]
frac_zealots =[0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95]

for beta in betas:
    for coverage in coverages:
        for vac_cost in vac_costs:
            for frac_conf in frac_confs:
                for frac_zealot in frac_zealots:
                    run_simulation(beta, coverage, vac_cost, frac_conf, frac_zealot, "c")