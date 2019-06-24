import networkx as nx
import subprocess
import csv
from multiprocessing import Pool

nrep = 1  # how many networks to average over
nwk_size = 1000
num_edges = 4 * nwk_size
seasons = 100  # defined in C file

fname = ".tmp_nwkfile"

# "usage: ./sir 1[nwk file] 2[beta] 3[initial coverage] \n"
# "4[cost of vaccination] 5[fraction of conformist] 6[fraction of zealot] <seed>\n"


def run_simulation(beta, coverage, vac_cost, frac_conf, frac_zealot):

    outbreak_avg = [0 for i in range(seasons)]
    time_to_extn_avg = [0 for i in range(seasons)]
    coverage_avg = [
        0 for i in range(seasons)
    ]  # this range is equal to # of seasons + 1
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
        )

        # out = subprocess.getoutput("./sir .tmp_nwkfile 2 0.8 2 0.4 0.1 0.2 0.9")
        out = subprocess.getoutput(command)
        # print(out)

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

    file_name = (
        "output1/"
        + "B="
        + str(beta)
        + "Cov="
        + str(coverage)
        + "VC="
        + str(vac_cost)
        + "conf="
        + str(frac_conf)
        + "z="
        + str(frac_zealot)
        + ".csv"
    )

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
                if i != 0:
                    writer.writerow(row)
                i += 1
            csvfile.close()

    except FileNotFoundError:
        print("Not found!")
        return 1


# "usage: ./sir 1[nwk file] 2[beta] 3[initial coverage] \n"
# "4[cost of vaccination] 5[fraction of conformist] 6[fraction of zealot] <seed>\n"

betas = [4.0]  # beta values to test
coverages = [0.9]
vac_costs = [0.6]  # btwn 0 to 1 (cost of falling illness is set to 1)
frac_confs = [0.1]
frac_zealots = [0.4, 0.45, 0.5, 0.55]

input_list = []

for beta in betas:
    for coverage in coverages:
        for vac_cost in vac_costs:
            for frac_conf in frac_confs:
                for frac_zealot in frac_zealots:
                    input_list.append(
                        (beta, coverage, vac_cost, frac_conf, frac_zealot)
                    )

print(input_list)

from multiprocessing import Pool

p = Pool(4)
p.starmap(run_simulation, input_list)
