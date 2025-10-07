#!/usr/bin/env bats

# Source and mount are
# cleaned up in setup
export SOURCE="$PWD/source"
export MOUNT="$PWD/mount"
export SOURCE_1="$PWD/source_1"
export SEA_LOG_FILE="${PWD}/sea.log"
@test "test" {
    for levels in {1..3}
    do
        load setup
        # careful, test is a bash internal
        bash -c "test -d ${MOUNT}/subdir"
        bash -c "test -f ${MOUNT}/file_in_source.txt"
        bash -c "test -f ${MOUNT}/subdir/file_in_subdir.txt"
        load unset
    done
}

@test "ls" {
    for levels in {1..3}
    do
        load setup
        for x in "${MOUNT} ${MOUNT}/file_in_source.txt ${MOUNT}/subdir ${MOUNT}/subdir/file_in_subdir.txt"
        do
        ls $x
        done
        a=$(ls ${MOUNT}) # ls dir content
        [[ $a == *"file_in_source.txt"* ]] # passthrough init message is in a

        if [[ $levels < 3 ]]
        then 
            [[ $a == *"file_in_mem.txt"* ]]
        fi
        load unset
    done
}

@test "ls duplicates" {
    for levels in {1..3}
    do
        load setup
        LD_PRELOAD="" cp ${SOURCE}/file_in_source.txt ${SOURCE_1}/file_in_source.txt
        a=$(ls ${MOUNT})
        [[ $(echo ${a} | grep -o "file_in_source.txt" | wc -l) == 1 ]]
        load unset
     done

}

@test "mkdir" {
    for levels in {1..3}
    do
        load setup
        mkdir ${MOUNT}/dir
        mkdir -p ${MOUNT}/subdir/a/b/c
        load unset
        test -d ${SOURCE}/dir
        test -d ${SOURCE}/subdir/a/b/c
        if [[ $levels < 3 ]]
        then 
            test -d ${SOURCE_1}/dir
            test -d ${SOURCE_1}/subdir/a/b/c
        fi
    done
}

@test "rm" {
    for levels in {1..3}
    do
        load setup
        rm ${MOUNT}/file_in_source.txt
        rm ${MOUNT}/subdir/file_in_subdir.txt
        if [[ $levels < 3 ]]
        then
            rm ${MOUNT}/file_in_mem.txt
            [ ! -f ${SOURCE_1}/file_in_mem.txt ]
        fi
        load unset
        [ ! -f ${SOURCE}/file_in_source.txt ]
        [ ! -f ${SOURCE}/subdir/file_in_subdir.txt ]
    done
}

@test "rmdir" {
    for levels in {1..3}
    do
        load setup
        mkdir -p ${MOUNT}/subdir/test1/test2
        rm -r ${MOUNT}/subdir
        load unset
        [ ! -d ${SOURCE}/subdir ]

        if [[ $levels < 3 ]]
        then
            [ ! -d ${SOURCE_1}/subdir ]
        fi
    done
}

@test "rm -rf" {
    for levels in {1..3}
    do
        load setup
        rm -rf ${MOUNT}/subdir
        load unset
        [ ! -d ${SOURCE}/subdir ]
        [ ! -d ${MOUNT}/subdir ]

        if [[ $levels < 3 ]]
        then
            [ ! -d ${SOURCE_1}/subdir ]
        fi

    done
}

@test "cp" {
    for levels in {1..3}
    do
        load setup
        ls mount/
        cp a.txt ${MOUNT}
        cp a.txt ${MOUNT}/subdir
        load unset
        test -f ${SOURCE}/a.txt
        test -f ${SOURCE}/subdir/a.txt
    done
}

@test "mv" {
    for levels in {1..3}
    do
        load setup
        mv a.txt ${MOUNT}
        mv ${MOUNT}/a.txt ${MOUNT}/subdir/a.txt
        load unset
        test -f ${SOURCE}/subdir/a.txt
        [ ! -f a.txt ]
    done
}

@test "dd" {
    for levels in {1..3}
    do
        load setup
        dd if=/dev/random of=${MOUNT}/file count=3
        load unset
        test -f ${SOURCE}/file
    done
}

@test "md5sum" {
    for levels in {1..3}
    do
        load setup
        m=$(md5sum ${MOUNT}/file_in_source.txt)
        [[ "$m" == *"3b5d5c3712955042212316173ccf37be"* ]]
        m=$(md5sum ${MOUNT}/subdir/file_in_subdir.txt)
        [[ "$m" == *"60b725f10c9c85c70d97880dfe8191b3"* ]]

        if [[ $levels < 3 ]]
        then
            m=$(md5sum ${MOUNT}/file_in_mem.txt)
            [[ "$m" == *"09fc7b9240aafc42736d8c59413bd67e"* ]]
        fi
        load unset
    done

}

@test "find" {
    for levels in {1..3}
    do
        load setup
        f=$(find ${MOUNT} -name file_in_subdir.txt)
        [[ "$f" == *"mount/subdir/file_in_subdir.txt" ]]
        f=$(find ${MOUNT} -name file_in_source.txt)
        [[ "$f" == *"mount/file_in_source.txt" ]]

        if [[ $levels < 3 ]]
        then
            f=$(find ${MOUNT} -name file_in_mem.txt)
            [[ "$f" == *"mount/file_in_mem.txt" ]]
        fi
        load unset
    done
}

@test "grep" {
    for levels in {1..3}
    do
        load setup
        grep b ${MOUNT}/file_in_source.txt
        grep a ${MOUNT}/subdir/file_in_subdir.txt

        if [[ $levels < 3 ]]
        then
            grep "seafile" ${MOUNT}/file_in_mem.txt
        fi
        load unset
    done
}

@test "file" {
    for levels in {1..3}
    do
        load setup
        f=$(file mount/file_in_source.txt)
        [[ "$f" == "mount/file_in_source.txt: ASCII text" ]]
        f=$(file mount/subdir/file_in_subdir.txt)
        [[ "$f" == "mount/subdir/file_in_subdir.txt: ASCII text" ]]

        if [[ $levels < 3 ]]
        then
            f=$(file mount/file_in_mem.txt)
            [[ "$f" == "mount/file_in_mem.txt: ASCII text" ]]
        fi
        load unset
    done
}

@test "tar" {
    for levels in {1..3}
    do
        load setup
        tar cvzf ${MOUNT}/foo.tar ${MOUNT}/file_in_source.txt
        tar cvzf ${MOUNT}/bar.tar ${MOUNT}/subdir/file_in_subdir.txt

        if [[ $levels < 3 ]]
        then
            tar cvzf ${MOUNT}/helloworld.tar ${MOUNT}/file_in_mem.txt
            test -f ${SOURCE}/helloworld.tar
        fi

        load unset
        test -f ${SOURCE}/foo.tar
        test -f ${SOURCE}/bar.tar
    done
}

@test "read python2" {
    if [ ! command -v python2 >/dev/null 2>&1 ]
    then
        for levels in {1..3}
        do
            load setup
            python2 tests/read.py ${MOUNT}/file_in_source.txt
            python2 tests/read.py ${MOUNT}/subdir/file_in_subdir.txt

            if [[ $levels < 3 ]]
            then
                python2 tests/read.py ${MOUNT}/file_in_mem.txt
            fi
            load unset
        done
    fi
}

@test "read python3" {
    for levels in {1..3}
    do
        type python3 || skip "Python 3 is not installed"
        load setup
        python3 tests/read.py ${MOUNT}/file_in_source.txt
        python3 tests/read.py ${MOUNT}/subdir/file_in_subdir.txt

        if [[ $levels < 3 ]]
        then
            python3 tests/read.py ${MOUNT}/file_in_mem.txt
        fi
        load unset
    done
}

@test "write python2" {
    if [ ! command -v python2 >/dev/null 2>&1 ]
    then
        for levels in {1..3}
        do
            load setup
            python2 tests/write.py ${MOUNT}/hello.txt
            python2 tests/write.py ${MOUNT}/subdir/hello.txt
            load unset
            test -f ${SOURCE}/hello.txt
            test -f ${SOURCE}/subdir/hello.txt
        done
    fi
}

@test "write python3" {
    for levels in {1..3}
    do
        type python3 || skip "Python 3 is not installed"
        load setup
        python3 tests/write.py ${MOUNT}/hello.txt
        python3 tests/write.py ${MOUNT}/subdir/hello.txt
        load unset
        test -f ${SOURCE}/hello.txt
    done
}

@test "string/numpy memmap python3" {
    for levels in {1..3}
    do
        load setup
        python3 tests/memmap.py ${MOUNT}/subdir/map.txt
        a=$(cat ${MOUNT}/subdir/map.txt)
        load unset
        [[ "$a" == "Heyyo World" ]]

        load setup
        python3 tests/mmap_numpy.py ${MOUNT}
        a=($(md5sum ${MOUNT}/updated.npy))
        b=($(md5sum ${MOUNT}/copied.npy))
        load unset
        [[ "$a" == "$b" ]]
    done
}

@test "chown" {
    for levels in {1..3}
    do
        id -u tmpu || skip "User tmpu not found"
        load setup
        a=$(chown tmpu:tmpu ${MOUNT}/file_in_source.txt)
        chown -R tmpu:tmpu ${MOUNT}/subdir

        if [[ $levels < 3 ]]
        then
            chown -R tmpu:tmpu ${MOUNT}/file_in_mem.txt
        fi
        load unset
    done
}

@test "chmod" {
    for levels in {1..3}
    do
        load setup
        chmod 600 ${MOUNT}/file_in_source.txt
        chmod -R g-rwx ${MOUNT}/subdir

        if [[ $levels < 3 ]]
        then
            chmod 600 ${MOUNT}/file_in_mem.txt
        fi
        load unset
    done
}

@test "cd" {
    for levels in {1..3}
    do
        load setup
        out=$(cd ${MOUNT};ls)
        if [[ $levels < 3 ]]
        then
            [[ $out == bin[[:space:]]complex_file.txt[[:space:]]file_in_mem.txt[[:space:]]file_in_source.txt[[:space:]]subdir ]]
        else
            [[ $out == bin[[:space:]]complex_file.txt[[:space:]]file_in_source.txt[[:space:]]subdir ]]
        fi
        load unset
    done
}

@test "which" {
    for levels in {1..3}
    do
        load setup
        bash -c "which ${MOUNT}/bin/hello.sh"
        load unset
    done
}

@test ">" {
    for levels in {1..3}
    do
        load setup
        bash -c "echo \"echo\" > ${MOUNT}/subdir/echo.txt"
        a=$(cat ${SOURCE}/subdir/echo.txt)
        [[ "$a" == "echo" ]]
        load unset
    done
}

@test "<" {
    for levels in {1..3}
    do
        load setup
        a=$(bash -c "bash < ${MOUNT}/bin/hello.sh")
        [[ "$a" == "hello" ]]
        load unset
    done
}

@test "awk" {
    for levels in {1..3}
    do
        load setup
        a=$(awk '$1=="a" {print $2}' ${MOUNT}/complex_file.txt)
        [[ "$a" == "b" ]]
        load unset
    done
}

@test "sed" {
    for levels in {1..3}
    do
        load setup
        sed -i s,b,a,g ${MOUNT}/file_in_source.txt
        a=$(cat ${MOUNT}/file_in_source.txt) # this also tests cat :)
        [[ "$a" == "a" ]]
        load unset
    done
}

@test "cut" {
    for levels in {1..3}
    do
        load setup
        a=$(cut -f 2 -d " " ${MOUNT}/complex_file.txt)
        [[ "$a" == "b" ]]
        load unset
    done
}

@test "unlink" {
    for levels in {1..3}
    do
        load setup
        unlink ${MOUNT}/file_in_source.txt
        unlink ${MOUNT}/subdir/file_in_subdir.txt

        if [[ $levels < 3 ]]
        then
            unlink ${MOUNT}/file_in_mem.txt
        fi

        load unset
    done
}

@test "rmtree" {

    type python3 || skip "Python 3 is not installed"
    mkdir -p actualdir
    touch actualdir/a.txt
    mkdir -p actualdir/d
    for levels in {1..3}
    do
        load setup
        mkdir -p ${SOURCE}/symdir
        ln -s actualdir/a.txt ${SOURCE}/symdir/a.txt 
        ln -s actualdir/d ${SOURCE}/symdir/d
        python3 tests/rmtree.py ${MOUNT}/symdir
        [ ! -d ${MOUNT}/symdir ]
        load unset
    done
}

@test "mkdirat" {
    for levels in {1..3}
    do
        load setup
        rm -rf ${MOUNT}/symdir
        mkdir -p ${MOUNT}/symdir
        load unset
        test -d ${SOURCE}/symdir

        if [[ levels < 3 ]]
        then
            test -d ${SOURCE_1}/symdir
        fi
    done

}

@test "symlink" {
    for levels in {1..3}
    do
        load setup
        mkdir -p ${MOUNT}/symdir
        ln -s actualdir/a.txt ${MOUNT}/symdir/a.txt 
        ln -s ${MOUNT}/file_in_mem.txt ${MOUNT}/symdir/b.txt
        load unset
    done

}

@test "access time" {
    for levels in {1..3}
    do
        load setup
        before=$(ls -lu ${SOURCE}/file_in_source.txt)
        # ensure at least 1 minute has changed
        sleep 61
        after=$(ls -lu ${MOUNT}/file_in_source.txt)

        [[ $before != $after ]] || echo "Before $before After $after"
        load unset
    done
}
