#!/bin/bash

infile=$1

start=`date +%s`
ls $infile
end=`date +%s`

pid=`echo $$`
host=`hostname`
echo "start_ls,${infile},${start},${pid}"
echo "end_ls,${infile},${start},${pid}"
