# Sea [![Build Status](https://travis-ci.com/ValHayot/Sea.svg?branch=master)](https://travis-ci.com/ValHayot/Sea)
![Sea logo](https://lh3.googleusercontent.com/_QD6PrqDMG50G8crgmnIa75jB3I4xnX1cREsHU-aQ0eTqAwBn4UoJ-NFwmVS6Q2xEXFqT2nQCNuhBf64NpuqDErX20Fd6iEKPAI1prmr)
Sea can provide the speed up of apache spark but with no instrumentation.
It is hierarchical filesystem implementation using LD_PRELOAD. Adapted from the passthrough found [XtreemOS](https://github.com/xtreemfs/xtreemfs).

## Compilation

`make pass`

## Usage

note: must first create a sea.ini configuration file
LD_PRELOAD=./passthrough.so SEA_HOME=$PWD ls mount
