#!/bin/bash


set -e
set -u

base_source=""
sources_arr=()
n_sources=0
conf_file=${SEA_HOME}/sea.ini
flush_file=${SEA_HOME}/.sea_flushlist

get_rp_mv () {
    tobe_flushed=$1
    OLDTIME=$2
    flushlist=""
    for f in $tobe_flushed
    do

        if [ ! -f $f ]
        then
            continue
        fi
        # code adapted from here https://stackoverflow.com/a/28341234
        # Get current and file times
        CURTIME=$(date +%s)
        FILETIME=$(stat $f -c %Y || true)

        if [[ $FILETIME == "" ]]
        then
            continue
        fi

        TIMEDIFF=$(($CURTIME - $FILETIME))

        # Check if file older
        if [ $TIMEDIFF -gt $OLDTIME ]; then
            echo "flushing and evicting $f to $base_source"
            #[ ! -f $f ] ||  &

            if [[ "$flushlist" != "" ]]
            then
                flushlist+=" "
            fi

            flushlist+=$f
           
        fi
        #file_out=$(echo "${f/$s/$base_source}")
        #for s in "${sources_arr[@]}"
        #do
            #if [[ "$f" == "${s}/"* ]]
            #then
            #fi
        #done
    done
    if [[ ${flushlist} != "" ]]
    then
        (mv $flushlist ${base_source} || true) &
        pid=$!
        wait $pid
        #echo "flush and evict ${flushlist}"
        #rsync -a --no-owner --no-group --remove-source-files ${flushlist} ${base_source} || ( sleep 5 && rsync -a --no-owner --no-group --remove-source-files ${flushlist} ${base_source} ) || echo "files cannot be flushed"
    fi
}

flush () {
        OLDTIME=$1
        re_flush=""
        sources_str=""
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

        if [[ $re_flush != "" ]]
        then
            found_files=""
            for rgx in ${re_flush}
            do
                if [[ $found_files != "" ]]
                then
                    found_files+=" "
                fi

                found_files+=$(find ${rgx} -type f -follow 2> /dev/null || true)
            done
            
            if [[ $found_files != "" ]]
            then
                tobe_flushed=$(ls -tur ${found_files} 2> /dev/null || true)
                get_rp_mv "${tobe_flushed}" "$OLDTIME"
            fi
        fi
}


flush_process () {
    OLDTIME=$1
    while :
    do
        flush $OLDTIME
        sleep 5 &
        wait $!
    done
}

cleanup () {
    echo "Cleaning up Sea mount"
    time flush 0
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

trap 'cleanup' SIGTERM
get_sources
flush_process 5

