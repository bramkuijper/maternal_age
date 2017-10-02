#!/usr/bin/env python3

# levelplot

import pandas as pd
import numpy as np
import scipy.ndimage 

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.lines as mlines
from matplotlib.ticker import AutoMinorLocator
from matplotlib import rcParams

plt.style.use('base')
rcParams['text.usetex'] = True 
rcParams['font.family'] = 'sans-serif'
rcParams['axes.titlesize'] = 14
rcParams['axes.labelsize'] = 14
rcParams['text.latex.preamble'] = [
       r'\usepackage{siunitx}',   # i need upright \micro symbols, but you need...
       r'\sisetup{detect-all}',   # ...this to force siunitx to actually use your fonts
       r'\usepackage{helvet}',    # set the normal font here
       r'\usepackage{sansmath}',  # load up the sansmath so that math -> helvet
       r'\sansmath'               # <- tricky! -- gotta actually tell tex to use!
]  

fig = plt.figure(figsize=(9,3))

# specify a grid with 3 rows and 2 columns
gs = gridspec.GridSpec(1, 2)

datafile = "summary_maternal_age_final.csv"
the_data = pd.read_csv(datafile, sep=";")

the_data["x1"] = 1.0 - the_data["x2"]

sub = the_data[(the_data["k"] == 0.5) & (the_data["y2"] == -1.02)]
sub = sub.sort_values(["x1"])

# initialize the plot
ax = plt.subplot(gs[0, 0])
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
ax.yaxis.set_ticks_position('left')
ax.xaxis.set_ticks_position('bottom')
ax.set_ylabel(r'Prop. phenotype $z_{1}$ offspring')
#ax.set_xlabel(r'Frequency environment $e_{1}$ patches, $f_{e_{1}}$')

ax.plot(sub["x1"], sub["p10"], "red", linewidth=1.0)
ax.plot(sub["x1"], sub["p11"]-0.0025, "#ff4f00", linewidth=1.0, linestyle=(0, (3.0,1.0)))
#ax.plot(sub["x1"], sub["p20"]-0.005, "blue", linewidth=1.5)
#ax.plot(sub["x1"], sub["p21"]-0.0075, "#00adff", linewidth=1.5, linestyle=(0, (3.0,1.0)))
minor_locator = AutoMinorLocator(4)
ax.xaxis.set_minor_locator(minor_locator)

minor_locator = AutoMinorLocator(5)
ax.yaxis.set_minor_locator(minor_locator)

ax.set_ylim((-0.05,1.05))
ax.set_title(r'Phenotype $z_{1}$ parents')

plt.text(x = 0.05,
        y = 0.9,
        s = "A",
        fontsize=15)

# initialize the plot
ax = plt.subplot(gs[0, 1])
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
ax.yaxis.set_ticks_position('left')
ax.xaxis.set_ticks_position('bottom')
#ax.set_xlabel(r'Frequency environment $e_{1}$ patches, $f_{e_{1}}$')

#ax.plot(sub["x1"], sub["p10"], "red", linewidth=1.0)
#ax.plot(sub["x1"], sub["p11"]-0.0025, "#ff4f00", linewidth=1.0, linestyle=(0, (3.0,1.0)))
ax.plot(sub["x1"], sub["p20"]-0.005, "blue", linewidth=1)
ax.plot(sub["x1"], sub["p21"]-0.0075, "#00adff", linewidth=1, linestyle=(0, (3.0,1.0)))
minor_locator = AutoMinorLocator(4)
ax.xaxis.set_minor_locator(minor_locator)

minor_locator = AutoMinorLocator(5)
ax.yaxis.set_minor_locator(minor_locator)

ax.set_ylim((-0.05,1.05))
plt.setp(ax.get_yticklabels(), visible=False)

ax.set_title(r'Phenotype $z_{2}$ parents')
plt.text(x = 0.05,
        y = 0.9,
        s = "B",
        fontsize=15)


# make custom legend, following http://matplotlib.org/users/legend_guide.html#proxy-legend-handles
red_line = mlines.Line2D(xdata = [ 0, 0.1 ], ydata = [ 1.0, 1.0 ], lw=1.0, color="red", label=r'$p_{y1}$: Young $z_{1}$ parents')
orange_line = mlines.Line2D(xdata = [ 0, 0.1 ], ydata = [ 1.0, 1.0 ], lw=1.0, color="#ff4f00", linestyle="--", label=r'$p_{o1}$: Old $z_{1}$ parents')
blue_line = mlines.Line2D(xdata = [ 0, 0.1 ], ydata = [ 1.0, 1.0 ], lw=1, color="blue", label=r'$p_{y2}$: Young $z_{2}$ parents')
lightblue_line = mlines.Line2D(xdata = [ 0, 0.1 ], ydata = [ 1.0, 1.0 ], lw=1, color="#00adff", linestyle="--", label=r'$p_{o2}$: Old $z_{2}$ parents')

plt.legend(
        handles=[red_line, orange_line, blue_line, lightblue_line],
        frameon=False,
        bbox_to_anchor=(2.0,1.0)
        )

plt.figtext(x = 0.35,
        y = -0.05,
        s = r'Frequency environment $e_{1}$-patches, $f_{e_{1}}$',
        fontsize=15)

plt.tight_layout()
graphtype = "pdf"
graphname = "matage_lineplot_ms." + graphtype
plt.savefig(graphname,format=graphtype, bbox_inches="tight")
