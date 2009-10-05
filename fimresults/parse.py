#!/usr/bin/python

fd = open ("fim_8_l1.out", "r")
input = [l.split() for l in fd.readlines()]

i = 0
while i < len (input):
  dataset = input[i][0]
  impl = input[i][1]
  l1_access = float(input[i][2])
  l2_access = float(input[i][3])
  sys_access = float(input[i][4])
  print dataset, impl, l2_access/(l2_access+sys_access), sys_access/(sys_access+l2_access)
  i = i + 1

#i = 0
#while i < len (input):
#  cur_string = input[i][0]
#  if cur_string[-4:] == "rsa:":
#    i = i + 1
#    for th in (2, 4, 6, 8, 10, 12):
#      print cur_string, th,":",
#      for impl in xrange(0, 3):
#        cur_value = 0
#        for j in xrange(0, 5):
#          cur_value += float(input[i][2])
#          i = i+1
#        print cur_value/5.0,
#      print
#  else:
#    i = i+1
