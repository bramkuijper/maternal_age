#!/usr/bin/env python3

import pandas as pd
import numpy as np
import scipy.ndimage 
import string

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.lines as mlines
from matplotlib.ticker import AutoMinorLocator
from matplotlib import rcParams
import matplotlib.colors as colors

import block
rcParams['text.usetex'] = True 
rcParams['font.family'] = 'sans-serif'
rcParams['axes.titlesize'] = '10'
rcParams['text.latex.preamble'] = [
       r'\usepackage{siunitx}',   # i need upright \micro symbols, but you need...
       r'\sisetup{detect-all}',   # ...this to force siunitx to actually use your fonts
       r'\usepackage{helvet}',    # set the normal font here
       r'\usepackage{sansmath}',  # load up the sansmath so that math -> helvet
       r'\sansmath'               # <- tricky! -- gotta actually tell tex to use!
]  

# load the datafile
# and calculate relevant variables 
datafile = "summary_maternal_age_duration.csv"
the_data = pd.read_csv(datafile, sep=";")

the_data["x1"] = 1 - the_data["x2"]

# function to remove tiny, irrelevant variations
# due to numerical solving
def func(row, var):
    x = row[var]
    if row[var] > 4:
        x=4
    elif row[var] < 0:
        x=0
    return x

the_data["qz1z1_old"] = the_data.apply(func,var="qz1z1_old",axis=1)
the_data["qz1z1_young"] = the_data.apply(func,var="qz1z1_young",axis=1)

vars = [ "qz1z1_old", "qz1z1_young" ]

# see http://stackoverflow.com/questions/18926031/how-to-extract-a-subset-of-a-colormap-as-a-new-colormap-in-matplotlib 
def truncate_colormap(cmap, minval=0.0, maxval=1.0, n=100):
    new_cmap = colors.LinearSegmentedColormap.from_list(
        'trunc({n},{a:.2f},{b:.2f})'.format(n=cmap.name, a=minval, b=maxval),
        cmap(np.linspace(minval, maxval, n)))
    return new_cmap

# set the figure size
fig = plt.figure(figsize=(10,7))

# specify a grid with 3 rows and 2 columns
widths = [ 1, 1, 1, 0.1 ]
heights = [ 1, 1 ]

# specify a grid with 3 rows and 2 columns
gs = gridspec.GridSpec(len(heights), len(widths), width_ratios=widths, height_ratios=heights)

b = block.Block(gs)

abc_ctr = 0

for i, var_i in enumerate(vars):

    # make the first block of the dataset
    data_sub = the_data[the_data["k"] == 0.1]
    row=i
    col=0

    new_cmap = plt.get_cmap('PuRd')

    title = r'Dispersal cost $k=0.1$'

    if row > 0:
        title = ""

    contourset = b.block(
            row=row,
            zlim=(0,4),
            col=col,
            yval=var_i,
            cmap=new_cmap,
            data=data_sub,
            x_tickval=row>0,
            y_tickval=col==0,
            ylab="",
            label=string.ascii_uppercase[abc_ctr],
            labelcolor="black",
            title=title)

    abc_ctr += 1

    data_sub = the_data[the_data["k"] == 0.5]
    row=i
    col=1
    
    
    title = r'Dispersal cost $k=0.5$'

    if row > 0:
        title = ""

    contourset = b.block(
            row=row,
            col=col,
            yval=var_i,
            data=data_sub,
            cmap=new_cmap,
            x_tickval=row>0,
            y_tickval=col==0,
            ylab="",
            label=string.ascii_uppercase[abc_ctr],
            labelcolor="black",
            title=title)

    abc_ctr += 1

    data_sub = the_data[the_data["k"] == 0.95]
    row=i
    col=2
    
    title = r'Dispersal cost $k=0.95$'

    if row > 0:
        title = ""

    contourset = b.block(
            row=row,
            col=col,
            yval=var_i,
            data=data_sub,
            cmap=new_cmap,
            x_tickval=row>0,
            y_tickval=col==0,
            ylab="",
            label=string.ascii_uppercase[abc_ctr],
            labelcolor="black",
            title=title)

    abc_ctr += 1

# reposition colorbar so that it does not narrow the last contourplot
# http://stackoverflow.com/questions/13784201/matplotlib-2-subplots-1-colorbar 
# http://matplotlib.org/api/figure_api.html#matplotlib.figure.Figure.add_axes
cbar_ax = plt.subplot(gs[0,3])

label = r"Expected duration of inheritance, $\log_{10} \left(E_{t} \right)$"
ticks = np.arange(0,4.5,0.5)

cbar = plt.colorbar(contourset,
        cax=cbar_ax,
        ticks=ticks)

cbar.ax.set_ylabel(label, fontsize=12)

plt.figtext(x = 0.45,
        y = -0.05,
        s = r'Frequency of environment $e_{1}$ patches, $f_{e_{1}}$',
        horizontalalignment='center',
        fontsize=15)

plt.figtext(x = -0.15,
        y = .9,
        s = r'Old parents $\rightarrow$',
        horizontalalignment='left',
        fontsize=15)

plt.figtext(x = -0.15,
        y = .07,
        s = r'Young parents $\rightarrow$',
        horizontalalignment='left',
        fontsize=15)

plt.figtext(x = -0.02,
        y = .5,
        s = r'Average rate of environmental change, $\bar{s}$',
        verticalalignment = 'center',
        rotation=90,
        fontsize=15)

plt.tight_layout()
graph_type = "pdf"
graphname = "levelplot_" + var_i + "." + graph_type
plt.savefig(graphname,format=graph_type, bbox_inches="tight")

