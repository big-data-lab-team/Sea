#!/bin/bash

set -u

run=$1

base_source=""
sources_arr=()
n_sources=0
conf_file=${SEA_HOME}/sea.ini
prefetch_file=${SEA_HOME}/.sea_prefetchlist
logging=0


re_prefetch=() # regex of files to prefetch
re_all=() # regex representing all files in directory

log () {
    if [[ logging -ge 3 ]]
    then
        echo "[Sea] $@"
    fi
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

assign_rgx () {
    re_all="${base_source}/*"

    # load all potential regex from file (concatenate the regex to existing source mounts)
    # load rgx of files that need to be prefetched
    tmp_reprefetch=("$(get_rgx ${base_source} ${prefetch_file})")
    reprefetch_arr=()
    IFS=' ' read -a reprefetch_arr <<< "${tmp_reprefetch}"
    re_prefetch=(${re_prefetch[@]+"${re_prefetch[@]}"} ${reprefetch_arr[@]+"${reprefetch_arr[@]}"})
}

prefetch () {

        prefetch_files="" # files to prefetch
        all_files="$(find -L ${re_all} -type f 2> /dev/null || true)" # all files in source mounts
        all_files+="$(find -L ${re_all} -type l 2> /dev/null || true)" # all files in source mounts

        # if .sea_prefetchlist file contains regex
        for rgx in ${re_prefetch[@]+"${re_prefetch[@]}"}
        do
            if [[ "${prefetch_files}" != "" ]]
            then
                prefetch_files+=" "
            fi
            prefetch_files+=$(echo ${all_files} | tr " " "\n" | grep -Eo "${rgx}" || true)
        done

        # check to see if there are any candidate files for flush/eviction and process them
        if [[ "${prefetch_files}" != "" ]]
        then
            for f in ${prefetch_files}
            do
                subpath=${f#${base_source}}

                for ((i=0;i<${n_lvls};i++)) {
                    FILESIZE=$(stat -c%s "$f")
                    AVAILSPACE=$(df --output=avail -B 1 "${sources_arr[0]}" | tail -n 1)

                    if [[ ${FILESIZE} -le ${AVAILSPACE} ]]
                    then
                        log "Prefetch: Prefetching $f to ${sources_arr[i]}${subpath}"
                        cp -H $f ${sources_arr[i]}${subpath} 2> /dev/null || true
                        break
                    fi
                }
            done
        fi
}


prefetch_process () {
    log "Prefetch: Starting prefetch thread"
    assign_rgx
    prefetch
    log "Prefetch: Prefetch process completed"
}

get_sources () {

    # refers to the last index and not the total number of sources
    n_lvls="$(($(awk -F "=" '/n_levels/ {print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;') - 1))"
    logging="$(awk -F "=" '/log_level/ {print $2}' ${SEA_HOME}/sea.ini | tr -d ' ;' | sed 's/#.*//g')"

    for ((i=0;i<${n_lvls};i++))
    do
        source_lvl=$(cat ${conf_file} | grep "^\s*cache_$i" | cut -d "=" -f 2 | tr -d ' ;')
        IFS=',' read -ra curr_sources <<< "$source_lvl"

        resolved_sources=()
        for src in "${curr_sources[@]}"
        do
            envvar=$(printenv | grep "^${src}=" | sed "s/^${src}=//g" || echo "")
            if [[ ${envvar} != "" ]]
            then
                resolved_sources+=( ${envvar} )
            else
                resolved_sources+=( ${src} )
            fi
        done
        sources_arr+=(${resolved_sources[@]})

        log "Prefetch: cache_$i ${resolved_sources[@]}"
    done

    base_source=$(cat ${conf_file} | grep "^\s*cache_$i" | cut -d "=" -f 2 | tr -d ' ;')

    envvar=$(printenv | grep "^${base_source}=" | sed "s/^${base_source}=//g" || echo "")
    if [[ ${envvar} != "" ]]
    then
        base_source=${envvar}
    fi

    log "Prefetch: Base dir ${base_source}"

}

if [[ ${run} == 1 ]]
then
    get_sources
    prefetch_process
fi
