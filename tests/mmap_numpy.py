#!/usr/bin/python3
import numpy as np
import sys


dirpath = sys.argv[1]
orig_fn = dirpath + "/updated.npy"
copy_fn = dirpath + "/copied.npy"

shape = (10, 10)
dtype = np.float32

rng = np.random.default_rng()
data = rng.random(size=shape, dtype=dtype)
np.save(orig_fn, data)

mmap_array = np.load(orig_fn, mmap_mode="r+")
mmap_array[0:2, 0] = 1 / 3
mmap_array.flush()

data[0:2, 0] = 1 / 3
cpy_array = data

np.save(copy_fn, cpy_array)
