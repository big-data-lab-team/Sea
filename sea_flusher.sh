#!/bin/bash

base_source=""
sources_arr=()
n_sources=0
flush_file=${SEA_HOME}/.sea_flushlist

flush () {
    while :
    do
        
        re_flush=""
        for s in $sources_arr
        do
            while IFS="" read -r re || [ -n "$re" ]
            do
                if [[ $re_flush != "" ]]
                then
                    re_flush+=" "
                fi

                re_flush+=$sources_arr/$re

            done < $flush_file
        done

        tobe_flushed=$(ls -tu $(find ${re_flush} -type f -follow -print))


        for f in $tobe_flushed
        do
            for s in $sources_arr
            do
                if [[ "$f" == "${s}/"* ]]
                then
                    file_out=$(echo "${f/$s/$base_source}")
                    echo "flushing and evicting $f to $file_out"
                    mv $f ${file_out}
                    break
                fi
            done
        done

        sleep 5
    done
}


get_sources () {

    # refers to the last index and not the total number of sources
    n_sources="$(($(awk -F "=" '/n_sources/ {print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;') - 1))"

    for ((i=0;i<${n_sources};i++))
    do
        # grep and cut
        sources_arr+=($(awk -v sc="source_$i" -F "=" '$0~sc{print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;'))
    done

    base_source="$(awk -v sc="source_$n_sources" -F "=" '$0~sc{print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;')"

}

trap 'trap - SIGTERM && kill 0' SIGINT SIGTERM EXIT
get_sources
flush
