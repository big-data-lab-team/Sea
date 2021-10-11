#!/bin/bash

export MOUNT="$PWD/mount"
export SOURCE="$PWD/source"
export SOURCE_1="$PWD/source_1"
export SEA_LOG_FILE="${PWD}/sea.log"
export SEA_HOME=${PWD}

gen_conf() {
cat > ${SEA_HOME}/sea.ini << DOC
# Sea configuration
[Sea]
mount_dir = ${MOUNT} ;
n_levels = 2 ;
cache_0 = ${SOURCE} ;
cache_1 = ${SOURCE_1}
log_level = 0 ; # 4 crashes tests
log_file = ${SEA_HOME}/sea.log ;
max_fs = 1048576 ;
DOC
}

setup () {
	gen_conf
}

@test "get_sources" {
	. bin/sea_prefetch.sh 0
	get_sources
	[[ ${sources_arr[@]} == ${SOURCE} ]]
	[[ ${base_source} == ${SOURCE_1} ]]
}

@test "get_rgx" {
	. bin/sea_prefetch.sh 0
	echo ".*prefetch.*" > ${SEA_HOME}/.sea_prefetchlist
	output=$(get_rgx ${SOURCE} ${SEA_HOME}/.sea_prefetchlist )

	[[ ${output} == "${SOURCE}/.*prefetch.*" ]]
}

@test "assign_rgx" {
	. bin/sea_prefetch.sh 0
	get_sources
	assign_rgx
	
	echo "re_prefetch ${re_prefetch[@]}"
	[[ "${re_all[@]}" == "${SOURCE_1}/*" && "${re_prefetch[@]}" == "${SOURCE_1}/.*prefetch.*" ]]

}

@test "prefetch" {
	. bin/sea_prefetch.sh 0
	get_sources
	assign_rgx

	f="${SOURCE_1}"/prefetch_f1.txt
	f2="${SOURCE_1}"/bin/prefetch_f2.sh
	f3="${SOURCE_1}"/subdir/prefetch_f3.txt
	touch ${f} ${f2} ${f3}
	
	prefetch_process

	[[ -f "${SOURCE}"/prefetch_f1.txt  && -f "${SOURCE}"/bin/prefetch_f2.sh && -f "${SOURCE}"/subdir/prefetch_f3.txt ]]
}

@test "prefetch symlinks" {
	rm ${SOURCE_1}/toprefetch1.txt ${SOURCE_1}/toprefetch2.txt ${SOURCE}/toprefetch1.txt ${SOURCE}/toprefetch2.txt 2> /dev/null || true
	. bin/sea_prefetch.sh 0
	get_sources
	assign_rgx

	mkdir -p inputs
	f=inputs/file1.txt
	f2=inputs/file2.txt
	echo "to prefetch" > $f
	echo "to prefetch" > $f2
	
	ln -s ${PWD}/$f ${SOURCE_1}/toprefetch1.txt 
	ln -s ${PWD}/$f2 ${SOURCE_1}/toprefetch2.txt 
	prefetch_process

	[[ -f "${SOURCE}"/toprefetch1.txt  && -f "${SOURCE}"/toprefetch2.txt ]]
}