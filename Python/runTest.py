# import sys
# sys.path.append('../ekp-debug/src/tests') # add library to the interpreter path!

from EKPSolvers import ekp_test
import os, re
import numpy as np

iPath = '../MinimalExamples/MiniExamples/'
files = os.listdir(iPath)
pattern = re.compile('[0-9]{3}.*\.txt$')
files = [f for f in files if (pattern.search(f) is not None)]
files.sort()

table = np.loadtxt(iPath + 'table.txt')
# print(table)
i = 0
for f in files:
  v = ekp_test(iPath + f)
  row = int(f.split('_')[0]) - 1
# print(f,'--> ','{:1.7f}'.format(a),'{:1.7f}'.format(b),'{:1.5e}'.format(abs(a-b)))
# print(f,v)

  if len(v) > 2 and v[5] < float("inf"):
    i = i + 1
    # a = v[4]
    # b = table[row, 2]
    print(f, '-->', 'k={:.0f} f={:.0f} x_f={:.0f} j={:.0f} ... fixed={:.0f} , unknown={:.0f}'.format(v[2], v[1], v[3], v[4],v[6],v[7]))
    # print(f, '--> ', '{:5.7f}'.format(a), '{:5.7f}'.format(b), '{:1.5e}'.format(abs(a - b)))
print('total: ', i)
