A hierarchical filesystem implementation using LD_PRELOAD. Adapted from the passthrough found [XtreemOS](https://github.com/xtreemfs/xtreemfs).

# Compilation

`make pass`

# Usage

note: must first create a sea.ini configuration file
LD_PRELOAD=./passthrough.so SEA_HOME=$PWD ls mount
