#!/bin/bash

set -e
set -u

conf_file=${SEA_HOME}/sea.ini

n_sources="$(($(awk -F "=" '/n_sources/ {print $2}' ${conf_file} | tr -d ' ;') - 1))"

for ((i=0;i<${n_sources};i++))
do
    source=($(cat ${conf_file} | grep "source_$i" | cut -d "=" -f 2 | tr -d ' ;'))
    mkdir ${source} 
done

#bash ${SEA_HOME}/sea_flusher.sh &
#pid=$!
#echo "flusher pid" ${pid}

echo "Launching script"
"$@"

#echo "killing flush"
#kill $pid
#trap 'trap - SIGTERM && kill 0' SIGINT SIGTERM EXIT
