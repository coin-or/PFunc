#!/usr/bin/python

import sys
import os
import stat
import fileinput
import re

# These are all the data files
threads = ['8']

# Boiler-plate
boilerplate = '\
=cluster;Cilk-style;Clustered\n\
=noupperright\n\
#=patterns\n\
legendx=4000\n\
legendy=800\n\
yformat=%g%%\n\
xlabel=Database Name\n\
extraops=set yrange [0:]\n\
ylabel=Runtime (Normalized to Cilk-style)\n\
title=\n\
=table\n'

for thread in threads:
  infilename = "fim_" + thread + ".out"
  outfilename = "fim_" + thread + ".perf"
  figfilename = "fim_" + thread + ".eps"
  indata = open (infilename, "r")
  outdata = open (outfilename, "w")
  print >>outdata, boilerplate
  for line in indata:
    tokens = line.split()
    input = tokens[0]
    cilk = float (tokens[2])
    pfunc = float (tokens[3])
    print >>outdata, input, (cilk/cilk)*100, (pfunc/cilk)*100
  indata.close()
  outdata.close()
  command = "bargraph.pl" + " " + outfilename + " > " + figfilename;
  print command
  os.system(command)
  command = "rm " + outfilename;
  os.system(command)
