#!/usr/bin/env python

import pandas as pd
import numpy as np
import itertools, math

# read in the data
data = pd.read_csv("summary_maternal_age.csv", sep=";")

# append a column
data["qz1z1"] = pd.Series(list(itertools.repeat(np.nan,len(data.index))))
data["qz2z2"] = pd.Series(list(itertools.repeat(np.nan,len(data.index))))

print("done")


def calc_z1(x):
    qz1 = ((x["f_1_1_1"] + x["f_2_0_0"])*(.5 * (x["p11"]+x["p10"]))+ 
            .5 * (x["f_1_0_1"] + x["f_2_1_0"])*x["p11"]+
            .5 * (x["f_1_1_0"] + x["f_2_0_1"])*x["p10"])/((x["f_1_1_1"] + x["f_2_0_0"]) + .5 *(x["f_1_0_1"] + x["f_2_1_0"] + x["f_1_1_0"] + x["f_2_0_1"]))

    return(math.log10(qz1/(1.0-qz1)))

def calc_z2(x):
    qz2 = ((x["f_1_0_0"] + x["f_2_1_1"])*(.5*(1.0-x["p21"])+.5*(1-x["p20"]))+ 
            .5 * (x["f_1_1_0"] + x["f_2_0_1"])*(1.0-x["p21"])+
            .5 * (x["f_1_0_1"] + x["f_2_1_0"])*(1.0-x["p20"]))/(x["f_1_0_0"] + x["f_2_1_1"] + .5 * (x["f_1_1_0"] + x["f_2_0_1"] + x["f_1_0_1"] + x["f_2_1_0"])
            )

    return(math.log10(qz2/(1.0-qz2)))

# calculate qz1z1
data["qz1z1"] = data.apply(calc_z1, axis=1)
data["qz2z2"] = data.apply(calc_z2, axis=1)

data.to_csv("summary_maternal_age_duration.csv",sep=";")
