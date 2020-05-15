#!/usr/bin/env bats

# Source and mount are
# cleaned up in setup
SOURCE="$PWD/source"
MOUNT="$PWD/mount"

@test "test" {
    load setup
    # careful, test is a bash internal
    bash -c "test -d ${MOUNT}/subdir"
    bash -c "test -f ${MOUNT}/file_in_source.txt"
    bash -c "test -f ${MOUNT}/subdir/file_in_subdir.txt"
}

@test "ls" {
    load setup
    for x in "${MOUNT} ${MOUNT}/file_in_source.txt ${MOUNT}/subdir ${MOUNT}/subdir/file_in_subdir.txt"
    do
      ls $x
    done
    a=$(ls ${MOUNT}) # ls dir content
    [[ $a == *"file_in_source.txt"* ]] # passthrough init message is in a
    load unset
}

@test "mkdir" {
    load setup
    mkdir ${MOUNT}/dir
    mkdir -p ${MOUNT}/subdir/a/b/c
    load unset
    test -d ${SOURCE}/dir
    test -d ${SOURCE}/subdir/a/b/c
}

@test "rm" {
    load setup
    rm ${MOUNT}/file_in_source.txt
    rm ${MOUNT}/subdir/file_in_subdir.txt
    load unset
    [ ! -f ${SOURCE}/file_in_source.txt ]
    [ ! -f ${SOURCE}/subdir/file_in_subdir.txt ]
}

@test "cp" {
    load setup
    ls mount/
    cp a.txt ${MOUNT}
    cp a.txt ${MOUNT}/subdir
    load unset
    test -f ${SOURCE}/a.txt
    test -f ${SOURCE}/subdir/a.txt
}

@test "mv" {
    load setup
    mv a.txt ${MOUNT}
    load unset
    test -f ${SOURCE}/a.txt
    [ ! -f a.txt ]
}

@test "dd" {
    load setup
    dd if=/dev/random of=${MOUNT}/file count=3
    load unset
    test -f ${SOURCE}/file
}

@test "rm -rf" {
    load setup
    rm -rf ${MOUNT}/subdir
    load unset
    [ ! -d ${SOURCE}/dir ]
    [ ! -d ${MOUNT}/dir ]
}

@test "md5sum" {
    load setup
    m=$(md5sum ${MOUNT}/file_in_source.txt)
    [[ "$m" == *"3b5d5c3712955042212316173ccf37be"* ]]
    m=$(md5sum ${MOUNT}/subdir/file_in_subdir.txt)
    [[ "$m" == *"60b725f10c9c85c70d97880dfe8191b3"* ]]

}

@test "find" {
    load setup
    f=$(find ${MOUNT} -name file_in_subdir.txt)
    [[ "$f" == *"mount/subdir/file_in_subdir.txt" ]]
    f=$(find ${MOUNT} -name file_in_source.txt)
    [[ "$f" == *"mount/file_in_source.txt" ]]
}

@test "grep" {
    load setup
    grep b ${MOUNT}/file_in_source.txt
    grep a ${MOUNT}/subdir/file_in_subdir.txt
}

@test "file" {
    load setup
    f=$(file mount/file_in_source.txt)
    [[ "$f" == "mount/file_in_source.txt: ASCII text" ]]
    f=$(file mount/subdir/file_in_subdir.txt)
    [[ "$f" == "mount/subdir/file_in_subdir.txt: ASCII text" ]]
}

@test "tar" {
    load setup
    tar cvzf ${MOUNT}/foo.tar ${MOUNT}/file_in_source.txt
    tar cvzf ${MOUNT}/bar.tar ${MOUNT}/subdir/file_in_subdir.txt
    load unset
    test -f ${SOURCE}/foo.tar
    test -f ${SOURCE}/bar.tar
}

@test "read python2" {
    load setup
    python2 tests/read.py ${MOUNT}/file_in_source.txt
    python2 tests/read.py ${MOUNT}/subdir/file_in_subdir.txt
}

@test "read python3" {
    type python3 || skip "Python 3 is not installed"
    load setup
    python3 tests/read.py ${MOUNT}/file_in_source.txt
    python3 tests/read.py ${MOUNT}/subdir/file_in_subdir.txt
}

@test "write python2" {
    load setup
    python2 tests/write.py ${MOUNT}/hello.txt
    python2 tests/write.py ${MOUNT}/subdir/hello.txt
    load unset
    test -f ${SOURCE}/hello.txt
    test -f ${SOURCE}/subdir/hello.txt
}

@test "write python3" {
    type python3 || skip "Python 3 is not installed"
    load setup
    python3 tests/write.py ${MOUNT}/hello.txt
    python3 tests/write.py ${MOUNT}/subdir/hello.txt
    load unset
    test -f ${SOURCE}/hello.txt
}