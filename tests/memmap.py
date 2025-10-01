#!/usr/bin/python3
import mmap
import sys


filename = sys.argv[1]

with open(filename, "w+") as f:
    f.write("Hello World")

with open(filename, "r+") as f:
    mm = mmap.mmap(f.fileno(), 5)
    mm[2:4] = "yy"
    mm.flush()
    mm.close()
