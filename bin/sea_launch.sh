#!/bin/bash

set -e
set -u

trap "exit" INT TERM
#trap "kill 0" EXIT

conf_file=${SEA_HOME}/sea.ini
curr_dir=$(dirname "$(readlink -f "$0")")
parent_dir=$(dirname ${curr_dir})

n_sources="$(($(awk -F "=" '/n_sources/ {print $2}' ${conf_file} | tr -d ' ;') - 1))"

for ((i=0;i<${n_sources};i++))
do
    source=($(cat ${conf_file} | grep "cache_$i" | cut -d "=" -f 2 | tr -d ' ;'))
    mkdir -p ${source} 
done

${parent_dir}/build/mirror
${curr_dir}/sea_prefetch.sh 1 &
${curr_dir}/sea_flusher.sh 1 &

pid=$!
echo "[Sea] Launch: flusher pid" ${pid}

trap 'trap - SIGTERM && kill $pid' SIGINT SIGTERM #EXIT

echo "[Sea] Launch: Launching script $@"
LD_PRELOAD=${parent_dir}/build/sea.so bash -c "$@"
echo "[Sea] Launch: Killing flush" && kill $pid

wait $pid

