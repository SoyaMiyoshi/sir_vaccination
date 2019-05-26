import networkx as nx
import subprocess
import csv

nrep = 1 # how many networks to average over
nwk_size = 5
num_edges = 10*nwk_size
seasons = 3 #defined in C file

fname = '.tmp_nwkfile'

def run_simulation(beta, coverage, efficacy, vac_cost, rationality, comformity, threshold):

    outbreak_avg = [0 for i in range(seasons )]
    time_to_extn_avg = [0 for i in range(seasons )]
    coverage_avg = [0 for i in range(seasons)]  # this range is equal to # of seasons + 1
    time = [i for i in range(seasons)]

    for rep in range(nrep):

        G = nx.gnm_random_graph(nwk_size, num_edges)  # generate network
        nx.write_edgelist(G, fname, data=False)

        command = "./sir .tmp_nwkfile" + " " + str(beta) + " " + str(coverage) + " " + str(efficacy) + " " \
                    + str(vac_cost) + " " + str(rationality) + " " + str(comformity) + " " + str(threshold)

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

    outbreak_avg = [float(i)/nrep for i in outbreak_avg]
    time_to_extn_avg = [float(i)/nrep for i in time_to_extn_avg]
    coverage_avg = [float(i)/nrep for i in coverage_avg]

    file_name = "/Volumes/Samsung_T5/vac_game1/" +"B="+ str(beta) + "Cov=" + str(coverage) + "Efic=" + str(efficacy) + "VC=" \
                    + str(vac_cost) + "R=" + str(rationality) + "C=" + str(comformity) + "Thr=" + str(threshold) + ".csv"

    try:
        with open(file_name,'w') as csvfile:
            print("writing", file_name)
            writer = csv.writer(csvfile)
            writer.writerow(["time", "avg outbreak size", "avg time to extinction", "coverage"])
            rows = zip(time, outbreak_avg, time_to_extn_avg, coverage_avg)
            for row in rows:
                writer.writerow(row)
            csvfile.close()

    except FileNotFoundError:
        print("Not found!")
        return 1


betas = [2.0] # beta values to test
coverages = [0.6]
efficacies = [4.0] # 1 means no medical efficacy
vac_costs = [0.9] # btwn 0 to 1
rationalities =[0.9] # low means less at-random
inverse_conformities = [0.9]  # low means less at-random
threshold_fractions = [0.9]

input_list = []

for beta in betas:
    for coverage in coverages:
        for efficacy in efficacies:
            for vac_cost in vac_costs:
                for rationality in rationalities:
                    for inverse_conformity in inverse_conformities:
                        for threshold_fraction in threshold_fractions:
                            input_list.append((beta, coverage, efficacy, vac_cost, rationality, inverse_conformity, threshold_fraction))

print(input_list)

from multiprocessing import Pool

p = Pool(4)
p.starmap(run_simulation, input_list)

i = 8