# import sys
# sys.path.append('../ekp-debug/src/tests') # add library to the interpreter path!

from EKPSolvers import SolveEKP_bb
import os, re
import numpy as np

iPaths = ['../MinimalExamples/001_SmallExamples/','../MinimalExamples/002_SmallExamples/','../MinimalExamples/003_SmallExamples/']

for iPath in iPaths:

  print('\n----- {:s} ----- \n'.format(iPath))

  files = os.listdir(iPath)
  pattern = re.compile('[0-9]{3}.*\.txt$')
  files = [f for f in files if (pattern.search(f) is not None)]
  files.sort()

  table = np.loadtxt(iPath + 'table.txt',skiprows=1)

  for i in range(table.shape[0]):
    t = table[i,3]
    r = SolveEKP_bb(iPath + files[i])
    print('{:02d} {:10.6f} {:10.6f} {:3.5e}'.format(i+1,t,r[1],abs(t-r[1])))
    if abs(t-r[1]) > 1e-10:
      print('ERROR')
      break

