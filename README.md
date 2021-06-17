# Sea [![Tests](https://github.com/big-data-lab-team/Sea/actions/workflows/test.yml/badge.svg)](https://github.com/big-data-lab-team/Sea/actions) [![codecov](https://codecov.io/gh/big-data-lab-team/Sea/branch/master/graph/badge.svg)](https://codecov.io/gh/big-data-lab-team/Sea)

![Sea logo](https://docs.google.com/drawings/d/e/2PACX-1vRasSXkeh1LjZuON2KRt1Yy0cgUyUEMPhAxqPiHKMD4-slBpeNzMXoBlWoHBhl9zWfI_dUrLTfsNKSj/pub?w=156&h=149)


Sea can provide the speed up of [Apache Spark](http://spark.apache.org) but with no instrumentation.

It is hierarchical filesystem implementation using LD_PRELOAD, adapted from
the passthrough of [XtreemOS](https://github.com/xtreemfs/xtreemfs).

## Requirements (if building from source)
- gcc-c++
- libiniparser
- gtest (optional)
- bash

## Compilation

`make pass`
or 
`make` (to compile tests)

## Usage
### Configuration file
In order to successfully run Sea alongside your application, you will need to provide some details in an `.ini` file called `sea.ini`.
The following are the properties of the file:
- `mount_dir`: This is the folder your application will directly interact with to access files in Sea. It is the "view" to Sea.
- `n_levels`: The number of source directory levels in your file system. Source directories are the directories that you would like Sea to use in order
               to speed up computation (e.g. tmpfs directories, local disk and Lustre)
- `source_<X>` : The path of the source. `<x>` is a digit representing at which level of the hierarchy this file system should be found at (starts at 0 - top).
                 For instance, a tmpfs path should likely be at `source_0` and a Lustre path should be found at the bottom of the hierarchy (e.g. `source_2`
                 if you have a local disk fs at `source_1`). These directories must exist before launching the application.
- `log_level` : This is an interger representing the amount of verbosity you desire in you application. There are currently 5 levels (0:None, 1:Error,
                2: Warning, 3: Info, 4: debug).
- `log_file` : location of file in which to store the logs
- `max_fs` : Pipeline maximum file size in Bytes. It is currently required that the maximum file size is passed to Sea in order for Sea to be able to determine
             if there is sufficient space in the source directions.
- `n_threads`: The maximum number of application concurrent threads or processes. Also used to determine if there is sufficient space.

An example configuration file may look like:
```                                                   
[Sea]                                                                  
mount_dir = /lustre/seamount ;                                    
n_levels = 3 ;                                                        
source_0 = /dev/shm/seasource ;                                           
source_1 = /localscratch/seasource ;                                         
source_2 = /lustre/seasource ;                                    
log_level = 0 ;                                 
log_file = /lustre/sea.log ;                             
max_fs = 646971392 ;                                                   
n_threads = 16 ;    
```

### Flushlist

Sea relies on a file called `.sea_flushlist` to determine which files need to be be flushed from the upper-level sources to the bottom-level source directory.
Exact filenames need not be provided and regex is accepted (can be thought or as similar to a `.gitignore`!). All filepaths resolved by the regex specified in the flushlist file will be flushed and evicted whenever possible.

Conversly, to free up space on storage, a `.sea_evictlist` files can be populated to contain files set for removal. Like the `.sea_flushlist`, this is a newline separated list of regex patterns.

Files that are listed in both the `.sea_evictlist` and `.sea_flushlist` are moved to the base source level filesystem (e.g. source_2 in the case above).


### Program execution

In order to launch your application with Sea, it is crucial to first set the `SEA_HOME` variable. This variable is used to indicated the folder in which the `sea.ini` and `.sea_flushlist` files are located.

The `sea_launch.sh` executable is used to launch the flushers and execute the program. However, your program must still set the `LD_PRELOAD` variable prior to execution in order to use the Sea file system. The LD_PRELOAD must be set with Sea's `passthrough.so`.

e.g.

`SEA_HOME=$PWD ./sea_launch.sh <myprogram>`

myprogram
```
#!/bin/bash
LD_PRELOAD=passthrough.so <myactualprogram>
```

### Troubleshooting

Some `coreutils` versions don't work with Sea because they do direct system 
calls instead of calling glibc versions. The following table summarizes 
our tests.

| coreutils version | Used in | Status in Sea |
| :------------- | :----------: | -----------: |
| 8.4            | CentOS 6     | WORKS |
| 8.22           | CentOS 7     | FAILS |
| 8.25           | Ubuntu Xenial | WORKS|
| 8.30           | CentOS 8     | FAILS |
| 8.31           | Fedora 30    | WORKS |

The Docker files 
used in the test cases show how to install specific `coreutils` versions from source.