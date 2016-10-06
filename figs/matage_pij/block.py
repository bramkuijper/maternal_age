#!/usr/bin/env python3

import pandas as pd
import numpy as np
import scipy.ndimage 

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.ticker import AutoMinorLocator

# set stylesheet
# should be present in matplotlib.get_configdir()/stylelib/$name.mplstyle
plt.style.use('base')

# single plot
class Block:

    def __init__(self, gridspec):

        self.gridspec_stuff = gridspec

    def block(self, row, col, data, yval="p11", x_tickval=False, y_tickval=False, title="", xlab="", ylab="", zlim=(-1,1), cmap="jet", label="A",label_loc=(0.12,0.25), labelcolor="white", interpolation="none"):

        global plt

        subdat_y = data[["x1","y2",yval]]

        # make pivot tables
        pivot_y = subdat_y.pivot_table(values=yval, index='y2', columns='x1')

        # make the grid for the levelplot
        xo = pivot_y.columns.values
        yo = pivot_y.index.values
        x, y = np.meshgrid(xo, yo)

        # initialize the plot
        ax = plt.subplot(
                self.gridspec_stuff[row, col]
                )

#        ax.set_xlim((0.01,.99))

        if not x_tickval:
            plt.setp(ax.get_xticklabels(), visible=False)

        if not y_tickval:
            plt.setp(ax.get_yticklabels(), visible=False)

        z = pivot_y.values

        step = float(zlim[1] - zlim[0])/100

        # contourplots of sex ratio
        out_ssr = ax.imshow(z,
                extent=[x.min(), x.max(), y.min(), y.max()],
                origin="lower",
                aspect="auto",
                interpolation=interpolation,
#                np.arange(zlim[0], zlim[1] + step, step),
                cmap=cmap
                )

        # set minor ticks
        minor_locator = AutoMinorLocator(5)
        ax.xaxis.set_minor_locator(minor_locator)
        minor_locator = AutoMinorLocator(5)
        ax.yaxis.set_minor_locator(minor_locator)

        if xlab != "":
            ax.set_xlabel(xlab)

        if ylab != "":
            ax.set_ylabel(ylab)

        # print the title
        if title != "":
            plt.title(title)

        # print the label text
        plt.text(x=label_loc[0],y=label_loc[1], s=label,fontsize=14, ha="center", va="center", color=labelcolor)

        return(out_ssr)
