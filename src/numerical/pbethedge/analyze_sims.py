#!/usr/bin/env python

import os, re, sys

first = True


def analyze_parameters(lines,first=False):

    pars = {}

    for line in lines:
        mobj = re.match("(.*);(.*)",line)
        if mobj != None:
            pars[mobj.group(1)] = mobj.group(2)

    return(pars)

def analyze_file(filename):

    global first;

    # open file; read data
    f = open(filename)
    fl = f.readlines()
    f.close

    if len(fl) < 3:
        return

    flhead = fl[0]

    lc = len(fl)
    parline = -1

    linerange = range(0,lc)
    linerange.reverse()

    # search the parameter line
    for lineno in linerange:

        if re.match("^c1",fl[lineno]) != None:
            parline = lineno
            break

    if parline == -1:
        return

    parameters = analyze_parameters(fl[parline:])

    datvals = fl[parline-3].strip()

    if first:
        print ";".join(parameters.keys()) + ";" + flhead.strip() + "file"
        first = False

    print ";".join(parameters.values()) + ";" + datvals + filename


def visit(arg, dirname, names):
    for name in names:
        if re.match("iter.*",name) != None:
            data = analyze_file(dirname + "/" + name)



os.path.walk(sys.argv[1], visit, None)
