#!/usr/bin/env python3

import pandas as pd
import numpy as np
import scipy.ndimage 

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
datafile = "summary_maternal_age_final.csv"
the_data = pd.read_csv(datafile, sep=";")

the_data["x1"] = 1 - the_data["x2"]
the_data["diffp1"] = the_data["p11"] - the_data["p10"]
the_data["diffp2"] = the_data["p21"] - the_data["p20"]
the_data = the_data.sort_values(["y2","x1"], ascending=[0,0])

# function to remove tiny, irrelevant variations
# due to numerical solving
def func(row, var):
    x = 0
    if abs(row[var]) > 1e-03:
        x=row[var]
    return x

the_data["diffp1"] = the_data.apply(func,var="diffp1",axis=1)
the_data["diffp2"] = the_data.apply(func,var="diffp2",axis=1)

vars = ["diffp1","diffp2"]
    
# see http://stackoverflow.com/questions/18926031/how-to-extract-a-subset-of-a-colormap-as-a-new-colormap-in-matplotlib 
def truncate_colormap(cmap, minval=0.0, maxval=1.0, n=100):
    new_cmap = colors.LinearSegmentedColormap.from_list(
        'trunc({n},{a:.2f},{b:.2f})'.format(n=cmap.name, a=minval, b=maxval),
        cmap(np.linspace(minval, maxval, n)))
    return new_cmap

for var_i in vars:

    # set the figure size
    fig = plt.figure(figsize=(10,3))

    # specify a grid with 3 rows and 2 columns
    widths = [ 1, 1, 1, 0.1 ]
    heights = [ 1 ]

    # specify a grid with 3 rows and 2 columns
    gs = gridspec.GridSpec(len(heights), len(widths), width_ratios=widths, height_ratios=heights)

    b = block.Block(gs)

    # make the first block of the dataset
    data_sub = the_data[the_data["k"] == 0.1]
    row=0
    col=0


    cmap = plt.get_cmap('jet')
    new_cmap = truncate_colormap(cmap, 0.05,0.95)

    contourset = b.block(
            row=row,
            col=col,
            yval=var_i,
            cmap=new_cmap,
            data=data_sub,
            x_tickval=True,
            y_tickval=True,
            ylab=r'Average rate of change, $\bar{s}$',
            label="A",
            title=r'Dispersal cost $k=0.1$')

    data_sub = the_data[the_data["k"] == 0.5]
    row=0
    col=1

    contourset = b.block(
            row=row,
            col=col,
            yval=var_i,
            data=data_sub,
            cmap=new_cmap,
            x_tickval=True,
            y_tickval=True,
            ylab="",
            label="B",
            title=r'Dispersal cost $k=0.5$')


    data_sub = the_data[the_data["k"] == 0.95]
    row=0
    col=2

    contourset = b.block(
            row=row,
            col=col,
            yval=var_i,
            data=data_sub,
            cmap=new_cmap,
            x_tickval=True,
            y_tickval=True,
            ylab="",
            label="C",
            title=r'Dispersal cost $k=0.95$')

    # reposition colorbar so that it does not narrow the last contourplot
    # http://stackoverflow.com/questions/13784201/matplotlib-2-subplots-1-colorbar 
    # http://matplotlib.org/api/figure_api.html#matplotlib.figure.Figure.add_axes
    cbar_ax = plt.subplot(gs[0,3])

    label = r"Age-difference, $p_{o1}-p_{y1}$"
    ticks = np.arange(0,1.1,0.1)
    
    if var_i == "diffp2":
        label = r"Age-difference, $p_{o2}-p_{y2}$"
        ticks = np.arange(-1.0,0.1,0.1)
    

    cbar = plt.colorbar(contourset,
            cax=cbar_ax,
            ticks=ticks)

    cbar.ax.set_ylabel(label, fontsize=12)

    plt.figtext(x = 0.45,
            y = -0.05,
            s = r'Frequency of environment $e_{1}$ patches, $f_{e_{1}}$',
            horizontalalignment='center',
            fontsize=15)

    plt.tight_layout()
    graph_type = "pdf"
    graphname = "levelplot_" + var_i + "." + graph_type
    plt.savefig(graphname,format=graph_type, bbox_inches="tight")

