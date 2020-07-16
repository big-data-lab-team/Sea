#!/bin/bash


set -e
set -u

base_source=""
sources_arr=()
n_sources=0
conf_file=${SEA_HOME}/sea.ini
flush_file=${SEA_HOME}/.sea_flushlist


get_rp_rsync () {
    tobe_flushed=$1
    flushlist=""
    for f in $tobe_flushed
    do
        for s in "${sources_arr[@]}"
        do
            if [[ "$f" == "${s}/"* ]]
            then
                file_out=$(echo "${f/$s/$base_source}")
                echo "flushing and evicting $f to $file_out"

                if [[ "$flushlist" != "" ]]
                then
                    flushlist+=" "
                fi
                flushlist+=$f
            fi
        done
    done
    rsync -a --no-owner --no-group --remove-source-files ${flushlist} ${base_source} || ( sleep 5 && rsync -a --no-owner --no-group --remove-source-files ${flushlist} ${base_source} ) || echo "files cannot be flushed"
}

flush () {
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

                found_files+=$(find ${rgx} -type f -follow || echo "")
            done
            
            if [[ $found_files != "" ]]
            then
                tobe_flushed=$(ls -tur ${found_files})
                get_rp_rsync "${tobe_flushed}"
            fi
        fi
}


flush_process () {
    while :
    do
        flush
        sleep 5 &
        wait $!
    done
}

cleanup () {
    echo "Cleaning up Sea mount"
    time flush
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

trap 'cleanup' SIGINT SIGTERM EXIT
get_sources
flush_process

