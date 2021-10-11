#!/bin/bash

#set -e
set -u
#set -x

run=$1

base_source=""
sources_arr=()
n_sources=0
conf_file=${SEA_HOME}/sea.ini
flush_file=${SEA_HOME}/.sea_flushlist
evict_file=${SEA_HOME}/.sea_evictlist
tmpfile=$(mktemp /tmp/.sea_flush.inprog.XXXXXX)
logging=0

# Regex arrays
re_flush=() # regex of files to flush
re_evict=() # regex of files to pin to local disk (don't flush or evict)
re_all=() # regex for all files

log () {
    if [[ logging -ge 3 ]]
    then
        echo "[Sea] $@"
    fi
}

fe_old () {

    # if we have no cache directories, flusher should terminate
    if [[ ${sources_arr[@]} == "" ]]
    then
        exit
    fi

    found_files=$1 # files that currently exist in source mounts
    OLDTIME=$2 # user specified expiration time
    task=$3 # mv / cp / rm (depending on which list)
    flist="" # list containing files for flush/eviction

    for f in ${found_files}
    do
        # code adapted from here https://stackoverflow.com/a/28341234
        # Get current and file times
        CURTIME=$(date +%s)
        FILETIME=$(stat $f -c %Y || true)

        if [[ $FILETIME == "" ]]
        then
            continue
        fi

        # calculate how old the file is (based on access time)
        TIMEDIFF=$(($CURTIME - $FILETIME))

        # Check if file older
        if [ $TIMEDIFF -ge $OLDTIME ]; then
            subpath=$f

            for s in "${sources_arr[@]}"
            do
                if [[ $f == *$s* ]]
                then
                    subpath=${subpath#$s}
                    subpath=$(echo $subpath | cut -f 2- -d '/' )
                    break
                fi
            done

            if [[ -f ${base_source}/${subpath} ]]
            then
                BASETIME=$(stat ${base_source}/${subpath} -c %Y || true)
                BASETIME=$(stat ${base_source}/${subpath} -c %Y || true)

                if [[ $BASETIME != NULL ]]
                then
                    TDIFF=$((${FILETIME} - ${BASETIME}))

                    if [[ ${TDIFF} == 0 ]]
                    then
                        continue 
                    fi
                fi

            fi

            if [[ $task != "rm" ]]
            then
                log "Flusher: $task $f to ${base_source}/${subpath}"
            fi
            # makedirectory in case it does not exist
            mkdir -p $(dirname ${base_source}/${subpath})
            # if we have some files that older that OLDTIME, flush/evict them
            # process each image individually (as opposed to all at once)
            if [[ $task == "mv" ]]
            then 
                mv $f ${base_source}/${subpath} 2> /dev/null || true
            elif [[ $task == "cp" ]]
            then 
                cp $f ${base_source}/${subpath} 2> /dev/null || true
            elif [[ $task == "rm" ]]
            then 
                log "Flusher: $task $f"
                rm $f 2> /dev/null || true
            fi
           
        fi
    done

}



get_rgx () {
    SOURCE=$1
    FILE=$2
    rgx=""
    
    if [ -f $FILE ]
    then
        while IFS="" read -r re || [ -n "$re" ]
        do
            if [[ $rgx != "" ]]
            then
                rgx+=" "
            fi

            rgx+="$SOURCE/$re"

        done < $FILE

    fi
    echo "${rgx}"
}

assign_rgx() {

    # load all potential regex from file (concatenate the regex to existing source mounts)
    for s in "${sources_arr[@]}"
    do
        # load rgx of files that need to be flushed
        tmp_reflush=("$(get_rgx $s ${flush_file})")
        reflush_arr=()
        IFS=' ' read -a reflush_arr <<< "${tmp_reflush}"
        re_flush=(${re_flush[@]+"${re_flush[@]}"} ${reflush_arr[@]+"${reflush_arr[@]}"})

        # do the same for files that need to be pinned
        tmp_reevict="$(get_rgx $s ${evict_file})"
        reevict_arr=()
        IFS=' ' read -a reevict_arr <<< "${tmp_reevict}"
        re_evict=(${re_evict[@]+"${re_evict[@]}"} ${reevict_arr[@]+"${reevict_arr[@]}"})

        re_all+=("$s/*")
    done
}

flush () {
    OLDTIME=$1 # user specified expiration time
    task=$2 # the name of the calling task. either "process" or "cleanup"

    flush_files="" # files to flush only
    evict_files="" # files to evict only
    fe_files="" # files to flush and evict
    all_files="" # all files in source mounts
    sources_str="" # string containing all source paths

    for rgx in "${re_all[@]}"
    do
        if [[ ${all_files} != "" ]]
        then
            all_files+=" "
        fi
        all_files+=$(find -L ${rgx} -type f 2> /dev/null || true)
        all_files+=$(find -L ${rgx} -type l 2> /dev/null || true)
    done

    # if .sea_flushlist file contains regex
    for rgx in ${re_flush[@]+"${re_flush[@]}"}
    do
        if [[ ${flush_files} != "" ]]
        then
            flush_files+=" "
        fi
        flush_files+=$(echo ${all_files} | tr " " "\n" | grep -Eo "${rgx}")
    done
    #echo "re_flush $re_flush"
    #echo "flush_files ${flush_files}"

    # if .sea_evictlist file contains regex
    for rgx in ${re_evict[@]+"${re_evict[@]}"}
    do
        if [[ ${evict_files} != "" ]]
        then
            evict_files+=" "
        fi

        if [[ $(ls ${rgx} 2> /dev/null) == "" ]]
        then
            continue
        fi
        evict_files+=$(echo ${all_files} | tr " " "\n" | grep -Eo "${rgx}" || true)
    done

    tmp_flush=""
    tmp_evict=""

    # find the files that only need to be flushed as well as those that need
    # to be flushed and evicted
    for ff in ${flush_files}
    do
        match=0
        for ef in ${evict_files}
        do
            if [[ ${ff} == ${ef} ]]
            then
                match=1

                if [[ ${fe_files} != "" ]]
                then
                    fe_files+=" "
                fi
                fe_files+=${ff}
                break
            fi
        done
        if [[ $match == 0 ]]
        then
            if [[ ${tmp_flush} != "" ]]
            then
                tmp_flush+=" "
            fi
            tmp_flush+=${ff}
        fi
    done

    # find files that only need to be evicted
    for ef in ${evict_files}
    do
        match=0
        for fe in ${fe_files}
        do
            if [[ ${ef} == ${fe} ]]
            then
                match=1
                break
            fi
        done
        if [[ ${match} == 0 ]]
        then
            if [[ ${tmp_evict} != "" ]]
            then
                tmp_evict+=" "
            fi
            tmp_evict+=${ef}
        fi
    done
    #echo ${fe_files}

    flush_files=${tmp_flush}
    evict_files=${tmp_evict}

    #echo "FLUSH ${flush_files}"
    #echo "EVICT ${evict_files}"

    # check to see if there are any candidate files for flush/eviction and process them
    if [[ ${fe_files} != "" ]]
    then
        fe_old "${fe_files}" "$OLDTIME" "mv"
    fi
    if [[ ${flush_files} != "" ]]
    then
        fe_old "${flush_files}" "$OLDTIME" "cp"
    fi
    if [[ ${evict_files} != "" ]]
    then
        fe_old "${evict_files}" "$OLDTIME" "rm"
    fi
}


flush_process () {
    OLDTIME=$1
    log "Flusher: Starting flusher"
    assign_rgx
    while [[ -f $tmpfile ]]
    do
        flush $OLDTIME "process"
        sleep 5 &
        wait $!
    done
    log "Flusher: Flush process terminated"
    log "Flusher: Starting cleanup"
    time flush 0 "cleanup"
}

cleanup () {
    log "Flusher: Cleaning up Sea mount"
    time flush 0 "cleanup"
}


get_sources () {

    # refers to the last index and not the total number of sources
    n_lvls="$(($(awk -F "=" '/n_levels/ {print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;') - 1))"
    logging="$(awk -F "=" '/log_level/ {print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;' | sed 's/#.*//g')"

    for ((i=0;i<${n_lvls};i++))
    do
        source_lvl=$(cat ${conf_file} | grep "^\s*cache_$i" | cut -d "=" -f 2 | tr -d ' ;')
        IFS=',' read -ra curr_sources <<< "$source_lvl"

        if [ ! -d ${curr_sources[@]} ]
        then
            sourcevar="\${curr_sources[@]}"
            curr_sources=$(eval echo "${sourcevar}")
        fi

        sources_arr+=(${curr_sources[@]})

        log "Flusher: source_$i ${curr_sources[@]}"
    done

    base_source=$(cat ${conf_file} | grep "^\s*cache_$i" | cut -d "=" -f 2 | tr -d ' ;')
    log "Flusher: base dir ${base_source}"

}

if [[ ${run} == 1 ]]
then
    trap '[ -f $tmpfile ] && rm $tmpfile' SIGTERM SIGINT EXIT
    get_sources
    flush_process 1
fi
