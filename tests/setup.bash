unset LD_PRELOAD # Clean up from previous tests
\rm -Rf ${SOURCE} ${MOUNT} # Clean up from previous tests
mkdir -p ${SOURCE} ${MOUNT} # Create source and mount (defined in tests.bats)
echo ${SOURCE} > sources.txt
echo a > a.txt # This file will be used in tests
echo b > ${SOURCE}/file_in_source.txt # This file will be used in tests
export LD_PRELOAD=${PWD}/passthrough.so