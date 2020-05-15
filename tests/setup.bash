unset LD_PRELOAD # Clean up from previous tests
\rm -Rf ${SOURCE} ${MOUNT} # Clean up from previous tests
mkdir -p ${SOURCE} ${MOUNT} # Create source and mount (defined in tests.bats)
echo ${SOURCE} > sources.txt
echo a > a.txt # This file will be used in tests
echo b > ${SOURCE}/file_in_source.txt # This file will be used in tests
mkdir ${SOURCE}/subdir
echo a > ${SOURCE}/subdir/file_in_subdir.txt
mkdir ${SOURCE}/bin # where fake binaries will be located
echo "#!/bin/bash" > ${SOURCE}/bin/hello.sh
echo "echo \"hello\"" > ${SOURCE}/bin/hello.sh
chmod +x ${SOURCE}/bin/hello.sh
export LD_PRELOAD=${PWD}/passthrough.so
