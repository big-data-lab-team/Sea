#!/usr/bin/python3
import mmap
import sys


filename = sys.argv[1]

with open(filename, "wb+") as f:
    f.write(b"Hello World")

with open(filename, "rb+") as f:
    mm = mmap.mmap(f.fileno(), 5)
    mm[2:4] = b"yy"
    mm.flush()
    mm.close()
