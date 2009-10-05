#!/usr/bin/python

import sys
import os
import stat
import fileinput
import re

# These are all the data files
threads = ['2', '4', '6', '8', '10', '12']

# Boiler-plate
boilerplate = '\
=cluster;Cilk;OpenMP;PFunc\n\
=noupperright\n\
#=patterns\n\
legendx=4000\n\
legendy=800\n\
yformat=%g%%\n\
xlabel=Matrix Name\n\
extraops=set yrange [60:]\n\
ylabel=Runtime (Normalized to Cilk)\n\
title=\n\
=table\n'

for thread in threads:
  infilename = "cg_" + thread + ".out"
  outfilename = "cg_" + thread + ".perf"
  figfilename = "cg_" + thread + ".eps"
  indata = open (infilename, "r")
  outdata = open (outfilename, "w")
  print >>outdata, boilerplate
  for line in indata:
    tokens = line.split()
    input = tokens[0]
    cilk = float (tokens[1])
    pfunc = float (tokens[2])
    omp = float (tokens[3])
    print >>outdata, input, cilk/cilk*100, omp/cilk*100.0, pfunc/cilk*100
  indata.close()
  outdata.close()
  command = "bargraph.pl" + " " + outfilename + " > " + figfilename;
  print command
  os.system(command)
  command = "rm " + outfilename;
  os.system(command)
