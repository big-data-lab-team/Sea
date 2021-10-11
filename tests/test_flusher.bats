#!/bin/bash

export MOUNT="$PWD/mount"
export SOURCE="$PWD/source"
export SOURCE_1="$PWD/source_1"
export SOURCE_2="$PWD/source_2"
export SEA_LOG_FILE="${PWD}/sea.log"
export SEA_HOME=${PWD}

mkdir -p ${SOURCE_2}/bin ${SOURCE_2}/build

gen_conf() {
cat > ${SEA_HOME}/sea.ini << DOC
# Sea configuration
[Sea]
mount_dir = ${MOUNT} ;
n_levels = 3 ;
cache_0 = ${SOURCE} ;
cache_1 = ${SOURCE_2}
cache_2 = ${SOURCE_1} ;
log_level = 0 ; # 4 crashes tests
log_file = ${SEA_HOME}/sea.log ;
max_fs = 1048576 ;
DOC
}

setup () {
	gen_conf
}

@test "get_sources" {
	. bin/sea_flusher.sh 0
	get_sources
	echo "caches ${sources_arr[@]}"
	[[ ${sources_arr[@]} == "${SOURCE} ${SOURCE_2}" ]]
	[[ ${base_source} == ${SOURCE_1} ]]
}

@test "get_rgx" {
	. bin/sea_flusher.sh 0
	echo ".*.txt" > ${SEA_HOME}/.sea_flushlist
	output=$(get_rgx ${SOURCE} ${SEA_HOME}/.sea_flushlist )

	echo ${output}
	[[ ${output} == "${SOURCE}/.*.txt" ]]
}

@test "cp" {
	. bin/sea_flusher.sh 0
	get_sources
	f="${SOURCE}"/cp.txt
	touch ${f}
	fe_old ${f} 0 cp 

	[[ -f "${SOURCE_1}"/cp.txt  && -f "${f}" ]]
        rm "${SOURCE_1}"/cp.txt ${f}
}

@test "mv" {
	. bin/sea_flusher.sh 0
	get_sources
	f="${SOURCE}"/mv.txt
	touch ${f}
	fe_old ${f} 0 mv

	[[ -f "${SOURCE_1}"/mv.txt  && ! -f "${f}"  ]]
	rm "${SOURCE_1}"/mv.txt

}

@test "rm" {
	. bin/sea_flusher.sh 0
	get_sources
	f="${SOURCE}"/rm.txt
	touch ${f}
	[[ -f ${f} ]]
	fe_old ${f} 0 rm

	[[ ! -f "${SOURCE_1}"/rm.txt  && ! -f "${f}" ]]
}

@test "assign_rgx" {
	. bin/sea_flusher.sh 0
	get_sources
	assign_rgx

	[[ "${re_all[@]}" == "${SOURCE}/* ${SOURCE_2}/*"  &&
	   "${re_flush[@]}" == "${SOURCE}/.*.txt ${SOURCE_2}/.*.txt" ]]
}

@test "flush" {
	rm .sea_evictlist 2> /dev/null || true
	. bin/sea_flusher.sh 0
	get_sources
	assign_rgx

	f1="f1.txt"
	f2="bin/f2.txt"
	f3="subdir/f3.txt"

	touch ${SOURCE}/${f1} ${SOURCE}/${f2} ${SOURCE}/${f3} ${SOURCE_2}/s4.txt
	flush 0 process

	[[ -f ${SOURCE_1}/${f1} && -f ${SOURCE_1}/${f2} && -f ${SOURCE_1}/${f3} ]]
	rm ${SOURCE_1}/${f1} ${SOURCE_1}/${f2} ${SOURCE_1}/${f3}
}

@test "evict" {
	. bin/sea_flusher.sh 0
	get_sources

	f1=${SOURCE}/bin/"script.sh"
	touch ${f1} 

	echo ".*.txt" > ${SEA_HOME}/.sea_evictlist
	echo "bin/.*" >> ${SEA_HOME}/.sea_evictlist

	#touch ${SOURCE_2}/bin/somefile

	assign_rgx
	flush 0 process

	[[ ! -f ${SOURCE}/${f1} &&
           ! -f ${SOURCE}/bin/script.sh &&
	   ! -f ${SOURCE}/bin/f2.txt && 
	   -f ${SOURCE}/subdir/f3.txt && 
	   -f ${SOURCE_1}/f1.txt && 
	   -f ${SOURCE_1}/bin/f2.txt && 
	   -f ${SOURCE_1}/subdir/f3.txt ]]

	rm ${SEA_HOME}/.sea_evictlist
	rm ${SOURCE}/subdir/f3.txt ${SOURCE_1}/f1.txt ${SOURCE_1}/bin/f2.txt ${SOURCE_1}/subdir/f3.txt
}

@test "timediff" {
	skip # currently has update issues
	. bin/sea_flusher.sh 0
	assign_rgx
	get_sources

	f1=bin/"timediff.txt"
	echo ${RANDOM} > ${SOURCE}/${f1} 
	cp ${SOURCE}/${f1} ${SOURCE_1}/${f1}

	a=$(flush 0 process)
	[[ $(echo ${a} | grep "cp ${SOURCE}/${f1}" | wc -l) == 0 ]]
	sleep 1

	echo "updated" > ${SOURCE}/${f1}
	a=$(flush 0 process)
	[[ $(echo ${a} | grep "cp ${SOURCE}/${f1}" | wc -l) == 1 ]]
}


