#!/bin/bash

gen_conf() {

    if [[ $1 == 1 ]]
    then
        cat > ${SEA_HOME}/sea.ini << DOC
        # Sea configuration
        [Sea]
        mount_dir = ${MOUNT} ;
        n_sources = 2 ;
        source_0 = ${SOURCE} ;
        source_1 = ${SOURCE_1}
        log_level = 3 ; # 4 crashes tests
        log_file = ${SEA_HOME}/sea.log ;
DOC
    else
        cat > ${SEA_HOME}/sea.ini << DOC
        # Sea configuration
        [Sea]
        mount_dir = ${MOUNT} ;
        n_sources = 1 ;
        source_0 = ${SOURCE} ;
        log_level = 3 ; # 4 crashes tests
        log_file = ${SEA_HOME}/sea.log ;
DOC
    fi

}

echo ${SOURCE}
unset LD_PRELOAD # Clean up from previous tests
\rm -Rf ${SOURCE} ${MOUNT} ${SOURCE_1} # Clean up from previous tests
mkdir -p ${SOURCE} ${MOUNT} ${SOURCE_1} # Create source and mount (defined in tests.bats)
echo a > a.txt # This file will be used in tests
echo b > ${SOURCE}/file_in_source.txt # This file will be used in tests
echo "seafile" > ${SOURCE_1}/file_in_mem.txt
mkdir ${SOURCE}/subdir
echo a > ${SOURCE}/subdir/file_in_subdir.txt
echo "a b" > ${SOURCE}/complex_file.txt
mkdir ${SOURCE}/bin # where fake binaries will be located
echo "#!/bin/bash" > ${SOURCE}/bin/hello.sh
echo "echo \"hello\"" > ${SOURCE}/bin/hello.sh
chmod +x ${SOURCE}/bin/hello.sh
export SEA_HOME=$PWD
gen_conf 2
export LD_PRELOAD=${PWD}/passthrough.so
