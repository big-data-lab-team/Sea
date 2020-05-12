\rm -Rf ${SOURCE} ${MOUNT} # Clean up from previous tests
mkdir -p ${SOURCE} ${MOUNT} # Create source and mount (defined in tests.bats)
echo ${SOURCE} > sources.txt
echo a > a.txt # This file will be used in tests
export LD_PRELOAD=passthrough.so