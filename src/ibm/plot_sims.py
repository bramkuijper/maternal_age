#!/usr/bin/env python

import sys, re, os.path, csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

import pandas as pd

from pylab import *

params={'axes.linewidth' : .5}
rcParams.update(params)

# get the filename from the command line
filename = sys.argv[1]

data = pd.read_csv(filename, sep=";",skiprows = 22)

nrows = data.shape[0]

row_each = math.floor(float(nrows)/1000)

# make dataset shorter so that we don't plot megabytes
data = data.iloc[range(0,nrows,int(row_each)),:]

## process the parameters at the end of the file
#def process_params(dictionary, rowctr):
#
#    fo = open(filename,"r")
#    fl = fo.readlines()
#
#    params = {};
#
#    for line in fl[rowctr:]:
#        if line.strip() != "":
#            splitted = line.strip().split(";")
#            params[splitted[0]] = splitted[1]
#
#    return params

# generate the figure

# initialize and specify size 
fig = plt.figure(figsize=(10,10))

num_rows = 2

# names of columns in dataframe
colnames = list(data.columns.values)

# add first subplot depicting % type 1 offspring
plt.subplot(num_rows,1,1)
if "meanp2" in colnames:
    plt.plot(data["generation"],data["meanp1"],'b',
            data["generation"],data["meanp2"],'r',linewidth=1)
    plt.legend((r'$p_{\mathrm{young}}$',r'$p_{\mathrm{old}}$'))
else:
    plt.plot(data["generation"],data["meanp10"],'c',
            data["generation"],data["meanp11"],'m',
            data["generation"],data["meanp21"],'y',
            data["generation"],data["meanp22"],'k',
            linewidth=1)
    plt.legend((r'$p_{1,\mathrm{young}}$',r'$p_{1,\mathrm{old}}$',r'$p_{2,\mathrm{young}}$',r'$p_{2,\mathrm{old}}$'))

plt.ylabel(r'prop. $z_{1}$ offspring')
plt.ylim(0,1)

# add 2nd subplot depicting patch frequencies
plt.subplot(num_rows,1,2)

# count number of patch frequencies
patch_freq_names = []
for name in colnames:
    if re.match("f_",name) != None:
        patch_freq_names.append(name)

colors = iter(cm.rainbow(np.linspace(0,1,len(patch_freq_names))))

for name in patch_freq_names:
    plt.plot(data["generation"],data[name],color=next(colors),linewidth=1)

#plt.tick_params(axis='x',which='both',bottom='on',top='on',labelbottom='off')
plt.ylabel(r'patch frequencies')
plt.legend(tuple(patch_freq_names))
plt.ylim(0,1)


graphname = os.path.dirname(filename)
if graphname != '':
    graphname += "/"
graphname += "graph_" + os.path.basename(filename) + ".pdf"

plt.savefig(graphname,format="pdf")
