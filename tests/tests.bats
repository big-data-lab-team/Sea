#!/usr/bin/env bats

# Source and mount are
# cleaned up in setup
SOURCE="source"
MOUNT="mount"

@test "ls" {
    load setup
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

@test "echo" {
    load setup
    echo 'c' > ${MOUNT}/c.txt
    load unset
    test -f ${SOURCE}/c.txt
    c=$(cat ${MOUNT}/c.txt)
    [[ $c == 'c' ]]
}
