#!/usr/bin/env python

import os, re, sys,math

from numpy import *

# frequency of envt 2
freq_patch_2 = list(arange(0.01,1.0,0.05)) + [ 0.9999]

# avarage switch rate
sbar = [ -0.98 ] #list(arange(-1.5, 0.5, 2.0/50))
k = [ 0.1, 0.5, 0.95 ]


c1 = 0.5
c2 = 0.5
c3 = 0.5
c4 = 0.5
C1 = 0
C2 = 0
C3 = 0
C4 = 0

exe = "./xevol_px"
#exe = "./xevol_p_bethedge"
#exe = "/home/uccoaku/maternal_age/src/ibm/xevol_p_bethedge"

counter = 0

reps = 10

# loop through x2 and y2
for f2 in freq_patch_2:
    for sbar_i in sbar:
        for k_i in k:
            for replicate_i in range(1,reps):
                s2 = sqrt(((1.0-f2)/f2) * 10**(2*sbar_i))
                s1 = 10**(2*sbar_i) / s2

                print("echo " + str(counter))
                counter += 1
                print(exe + " " + str(s1) + " " + str(s2) + " " 
                        + str(c1) + " " + str(c2) + " "
                        + str(C1) + " " + str(C2) + " "
                        + str(c3) + " " + str(c4) + " "
                        + str(C3) + " " + str(C4) + " "
                        + str(k_i) + " 0.01 0.02 " + " 0.5 0.5 ")
#    s[0] = atof(argv[1]);
#    s[1] = atof(argv[2]);
#    c_young[0] = atof(argv[3]);
#    c_young[1] = atof(argv[4]);
#    C_young[0] = atof(argv[5]);
#    C_young[1] = atof(argv[6]);
#    c_old[0] = atof(argv[7]);
#    c_old[1] = atof(argv[8]);
#    C_old[0] = atof(argv[9]);
#    C_old[1] = atof(argv[10]);
#    k = atof(argv[11]);
#    mu_h = atof(argv[12]);
#    sdmu_h = atof(argv[13]);
#    d0 = atof(argv[14]);
#    h0 = atof(argv[15]);
