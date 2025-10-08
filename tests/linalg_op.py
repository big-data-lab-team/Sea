import numpy as np
import sys

filename = sys.argv[1] + "/tensordot.npy"
a = np.load("a.npy")
b = np.load("b.npy")

tdot = np.tensordot(a, b, axes=([1, 0], [0, 1]))

np.save(filename, tdot)
