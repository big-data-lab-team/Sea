#!/usr/bin/env python3

import subprocess
import random
import sys

# to be executed from root directory

it = int(sys.argv[1])

experiments = ['native', 'ld']
experiments = experiments * it
random.shuffle(experiments)


native_cmd = "benchmarks/scripts/bench_disks.sh /dev/shm/native.txt benchmarks/data/bench_native.csv"
ld_cmd = "LD_PRELOAD=./passthrough.so benchmarks/scripts/bench_disks.sh ./mount/ld.txt benchmarks/data/bench_ld.csv"

for exp in experiments:
    if exp == 'native':
        p = subprocess.Popen(native_cmd, shell=True)
        p.communicate()

    else:
        p = subprocess.Popen(ld_cmd, shell=True)
        p.communicate()



