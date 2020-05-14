#!/usr/bin/env bats

# Source and mount are
# cleaned up in setup
SOURCE="source"
MOUNT="mount"

@test "ls" {
    load setup
    ls ${MOUNT}/file_in_source.txt
    a=$(ls ${MOUNT})
    [[ $a == *"file_in_source.txt" ]] # passthrough init message is in a
    load unset
}

@test "mkdir" {
    load setup
    mkdir ${MOUNT}/dir
    load unset
    test -d ${SOURCE}/dir
}

@test "rm" {
    load setup
    rm ${MOUNT}/file_in_source.txt
    load unset
    [ ! -f ${SOURCE}/file_in_source.txt ]
}

@test "cp" {
    load setup
    ls mount/
    cp a.txt ${MOUNT}
    load unset
    test -f ${SOURCE}/a.txt
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
    mkdir ${SOURCE}/dir
    echo a > ${SOURCE}/dir/a.txt
    load setup
    rm -rf ${MOUNT}/dir
    load unset
    [ ! -d ${SOURCE}/dir ]
    [ ! -d ${MOUNT}/dir ]
}

@test "md5sum" {
    load setup
    m=$(md5sum ${MOUNT}/file_in_source.txt)
    echo $m
    [[ "$m" == *"3b5d5c3712955042212316173ccf37be"* ]]

}

@test "find" {
    load setup
    f=$(find ${MOUNT} -name file_in_source.txt)
    [[ "$f" == *"mount/file_in_source.txt" ]]
}

@test "grep" {
    load setup
    grep b ${MOUNT}/file_in_source.txt
}

@test "file" {
    load setup
    f=$(file mount/file_in_source.txt)
    [[ "$f" == "mount/file_in_source.txt: ASCII text" ]]
}

@test "read python" {
    load setup
    a=$(python tests/read.py ${MOUNT}/file_in_source.txt)
}

@test "write python" {
    load setup
    python tests/write.py ${MOUNT}/hello.txt
    load unset
    test -f ${SOURCE}/hello.txt
}