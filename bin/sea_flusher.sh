#!/bin/bash

#set -e
set -u
#set -x

base_source=""
sources_arr=()
n_sources=0
conf_file=${SEA_HOME}/sea.ini
flush_file=${SEA_HOME}/.sea_flushlist
tmpfile=$(mktemp /tmp/.sea_flush.inprog.XXXXXX)

mv_old () {

    found_files=$1 # files that currently exist in source mounts
    OLDTIME=$2 # user specified expiration time
    task=$3 # the name of the calling task (either "process" or "cleanup")

    flushlist=""
    for f in $found_files
    do
        
        # check if file exists otherwise continue (in case it has been flushed by another process)
        if [ ! -f $f ]
        then
            continue
        fi
        # code adapted from here https://stackoverflow.com/a/28341234
        # Get current and file times
        CURTIME=$(date +%s)
        FILETIME=$(stat $f -c %Y || true && continue)

        if [ $FILETIME == "" ]
        then
            continue
        fi

        # calculate how old the file is (based on access time)
        TIMEDIFF=$(($CURTIME - $FILETIME))

        # Check if file older
        if [ $TIMEDIFF -ge $OLDTIME ]; then
            echo "flushing and evicting $f to $base_source"

            # some string formatting
            if [[ "$flushlist" != "" ]]
            then
                flushlist+=" "
            fi

            flushlist+=$f
           
        fi
    done

    # if we have some files that older that OLDTIME, move them
    if [[ ${flushlist} != "" ]]
    then

        # mv each image individually (as opposed to all at once)
        for f in $flushlist
        do
            ([ ! -f $f ] || mv $f ${base_source} || true) &
        done

        # wait for all images to be moved
        wait

        #(mv $flushlist ${base_source} || true) &
        #pid=$!
        #wait $pid # wait until subprocess completes
    fi
}

flush () {
        OLDTIME=$1 # user specified expiration time
        task=$2 # the name of the calling task. either "process" or "cleanup"

        re_flush="" # regex of files to flush
        sources_str="" # string containing all source paths

        # load all potential regex from file (concatenate the regex to existing source mounts)
        for s in "${sources_arr[@]}"
        do
            while IFS="" read -r re || [ -n "$re" ]
            do
                if [[ $re_flush != "" ]]
                then
                    re_flush+=" "
                fi

                re_flush+=$s/$re

            done < $flush_file
        done

        # if .sea_flushlist file contains regex
        if [[ $re_flush != "" ]]
        then
            found_files=""

            # Look for files that match provided regex patterns
            for rgx in ${re_flush}
            do
                if [[ $found_files != "" ]]
                then
                    found_files+=" "
                fi
                found_files+=$(find ${rgx} -type f -follow 2> /dev/null || true)
            done
            
            # check to see if there are any candidate files for flushing and mv them
            if [[ $found_files != "" ]]
            then
                mv_old "${found_files}" "$OLDTIME" "$task"
            fi
        fi
}


flush_process () {
    OLDTIME=$1
    while [[ -f $tmpfile ]]
    do
        flush $OLDTIME "process"
        sleep 5 &
        wait $!
    done
    echo "flush process terminated"
}

cleanup () {
    echo "Cleaning up Sea mount"
    sp=$(jobs -p)
    [[ $sp != "" ]] && kill -INT $sp
    rm $tmpfile
    ps aux | grep vhs
    time flush 0 "cleanup"
}


get_sources () {

    # refers to the last index and not the total number of sources
    n_lvls="$(($(awk -F "=" '/n_levels/ {print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;') - 1))"

    echo $n_lvls
    for ((i=0;i<${n_lvls};i++))
    do
        source_lvl=$(cat ${conf_file} | grep "^\s*source_$i" | cut -d "=" -f 2 | tr -d ' ;')
        IFS=',' read -ra curr_sources <<< "$source_lvl"

        sources_arr+=(${curr_sources[@]})

        echo source_$i ${sources_arr[@]}
    done

    base_source=$(cat ${conf_file} | grep "^\s*source_$i" | cut -d "=" -f 2 | tr -d ' ;')
    echo base dir ${base_source}

}

trap '[ -f $tmpfile ] && cleanup' SIGTERM 
get_sources
flush_process 5
