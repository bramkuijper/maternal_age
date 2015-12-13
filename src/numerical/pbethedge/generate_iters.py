#!/usr/bin/env python

import os, re, sys,math

from numpy import *

# frequency of envt 2
freq_patch_2 = list(arange(0.01,0.99,0.02))

# avarage switch rate
sbar = [ -1.0 ]#list(arange(-1.5, 0.5, 2.0/50))
k = [0.95]#[ 0.1, 0.5, 0.95 ]

#exe = "/home/uccoaku/fecmort/src/fecsel_bethedge/numsolve"
#exe = "/home/uccoaku/msparentaleffects1/numsolve"
exe = "./numsolve"


ctr = 0

c1 = 0.5
c2 = 0.5
c3 = 0.8
c4 = 0.8
i = 0.0
t = 0.0
eul = 0.01
q1 = 0
q2 = 0
Q = 0.05
mt_init = 0.5
p_init = "0.5 0.5"

max_iter = 1000000

# initial values for patch freqs and relatedness coeffs
fval_init = " ".join([ str(1.0/8) for xi in range(0,8) ])

# initial values for the reproductive values
vval_init = " ".join([ str(1.0) for xi in range(0,16) ])

# initial values for the relatedness coefficients 
relval_init = " ".join([str(xi) for xi in [0.5]*8])

for f2 in freq_patch_2:
    for sbar_i in sbar:
        for k_i in k:
            s2 = sqrt(((1.0-f2)/f2) * 10**(2*sbar_i))
            s1 = 10**(2*sbar_i) / s2
            print("echo " + str(ctr))
            print(exe + " " + str(max_iter) + " " + str(c1) + " " + str(c2) + " " + str(c3) + " " + str(c4) + " " + str(q1) + " " + str(q2) + " "  + str(k_i) + " " + str(mt_init) + " " + str(s1) + " " + str(s2) + " " + str(Q) + " " + p_init + " " + fval_init + " " + vval_init + " " + relval_init)
            ctr+=1
        
