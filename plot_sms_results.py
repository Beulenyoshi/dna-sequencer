#!/usr/bin/env python3
#
# * ===========================================================================
# *
# * Philipp Schubert
# *
# *    Copyright (C) 2017
# *    Software Engineering Group
# *    Heinz Nixdorf Institut
# *    University of Paderborn
# *    philipp.schubert@upb.de
# *
# * ===========================================================================
#

import sys
import csv
import numpy as np
import matplotlib.pyplot as plt

if len(sys.argv) != 2:
	print("usage: prog <input csv>")
	exit(1)

print("Assure you have matplotlib installed: pip install matplotlib")

input = sys.argv[1]

with open(input) as f:
    reader = csv.reader(f)
    header = next(reader)
    x = []
    y = []
    for row in reader:
        x.append(row[0])
        y.append(row[1])

plt.scatter(x, y, color='r', s=50, marker='x')
plt.xlabel(header[0], fontsize=18)
plt.ylabel(header[1], fontsize=18)
plt.show()

exit(0)
