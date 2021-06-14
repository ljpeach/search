import subprocess
import argparse as ap
import os
from collections import OrderedDict

argParser = ap.ArgumentParser(description = 'Solve a Blocksworld puzzle. use both -b and -f to create a new puzzle with b blocks, and write to file f.')
argParser.add_argument('a', type = str, help = "algorithm name")
argParser.add_argument('-mem', type = str, help = "memory bound")
argParser.add_argument('-walltime', type = int, help = "time bound")
argParser.add_argument('domain', type = str, help = "Name of domain")
argParser.add_argument('n', type = int, help = "Number of blocks.")
argParser.add_argument('-dir', type = str, help = "folder to fetch from")
argParser.add_argument('-iter', type = int, help = "number of iterations")
arguments = argParser.parse_args()

algo = arguments.a
mem = arguments.mem
if mem == None:
    mem = ""
walltime = arguments.walltime
if walltime == None:
    walltime = ""
nBlock = arguments.n
if nBlock == None:
    walltime = None
domain = arguments.domain
dir = arguments.dir
if dir == None:
    dir = "./"
iter = arguments.iter
if iter == None:
    iter = 10
os.system("rm tests.txt")
cmdStr = "python problemGen.py -b " + str(nBlock) + " -f " + str(nBlock) + "BlockTest; "+ dir + str(nBlock) + domain + "_solver " + algo +" -mem " + mem + " -walltime " + str(walltime) + " < " + str(nBlock) + "BlockTest"
outDict = OrderedDict()
print("Testing: " + domain + " using: "+ algo)
for i in range(iter):
    process = subprocess.Popen("echo Test Number "+str(i+1)+ " >> tests.txt; cat " + str(nBlock) + "BlockTest >> tests.txt; " +cmdStr, shell = True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.stdout, process.stderr
    lines = [line.decode("utf-8") for line in stdout.readlines()]
    for i in lines:
        if i[:5] == "#pair":
            #keystart = 8
            importantIndices = [0,0]
            bit = 0
            for j in range(8, len(i)):
                if i[j]=='"':
                    importantIndices[bit] = j
                    bit+=1
                    if bit==2:
                        break
            key = i[8:importantIndices[0]]
            item = i[importantIndices[1]+1:-2]
            try:
                item = float(item)
                if key not in outDict:
                    outDict[key] = []
                outDict[key].append(item)
            except:
                if key not in outDict:
                    outDict[key] = item
#for i in outDict:
#    print(i + " : " + str(outDict[i]))

print("Average wall time: " + str(sum(outDict["total wall time"])/len(outDict["total wall time"])))
print("Average expanded: " + str(sum(outDict["total nodes expanded"])/len(outDict["total nodes expanded"])))
print("Average generated: " + str(sum(outDict["total nodes generated"])/len(outDict["total nodes generated"])))
