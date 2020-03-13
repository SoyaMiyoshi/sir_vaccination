# 実現したいこと
# オアシスで時系列データの閲覧(サーバーでrunする)

# オアシスから受け取るべき入力
# 1 "gnm" or "ba"
# 2 nwk_size
# 3 egdes
# 4 beta
# 5 initial_coverage
# 6 vaccination_cost
# 7 rationality
# 8 memory length
# 9 seed(オアシスから自動的に入力される)

from sys import argv
from random import *
from scipy import stats
from subprocess import check_output
import json

seed(int(argv[9]))

if(argv[1] not in ['ba', 'gnm']):
    raise ValueError('wrong argv[1]')

if(not str.isdigit(argv[8])):
    raise ValueError('argv[8] should be integer')

data = {}
fname = "/home/sou/j/vg-master/res/" + argv[1] + argv[2] + "edges" + argv[3] + "id" + 1
l = check_output(['./sir',fname, argv[4], argv[5], argv[6], argv[7], argv[8], "b" ,str(getrandbits(64))]).decode("utf-8").strip() # call sir
for line in l.split("\n"):
    tmp = line.split(" ")
    data[int(tmp[0])] = {"Coverage": tmp[1], "Outbreak": tmp[2]}

with open("_output.json", mode='w') as f:
    json.dump(data, f)