#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: ./instrument-problem1 Targets_file Prj_dir Build_dir" >&2
    echo "-----------------------------help information----------------------------"
    echo "  Targets_file is: the file that contains all instrumentation locations"
    echo "       Prj_dir is: the directory where source code lies"
    echo "     Build_dir is: the directory that stores instrumented code"
    exit
fi

Targets_file=$1   
Prj_dir=$2  
Build_dir=$3

export AFLGO=$LTLFuzzer/AFLGo

cd $Build_dir
while IFS=":" read -r fileName lineNum event
do
    if [ -d  $fileName":"$lineNum ]; then
        rm -rf $fileName":"$lineNum
    fi
    mkdir $fileName":"$lineNum
    Binary_DIR=$(realpath $fileName":"$lineNum)
    cd $Binary_DIR
	cp -r $Prj_dir/* .
	cd ./examples/telnet-server
    mkdir TMP 
    TMP_DIR=$(realpath TMP)

    echo $fileName":"$lineNum >$TMP_DIR/BBtargets.txt
	
    export COMPILE_ADDITIONAL=" -targets=$TMP_DIR/BBtargets.txt -outdir=$TMP_DIR -flto -fuse-ld=gold -Wl,-plugin-opt=save-temps"
	export LINK_ADDITIONAL="-targets=$TMP_DIR/BBtargets.txt -outdir=$TMP_DIR -flto -fuse-ld=gold -Wl,-plugin-opt=save-temps"
    export CC="$AFLGO/afl-clang-fast $COMPILE_ADDITIONAL"
	export CXX="$AFLGO/afl-clang-fast++ $LINK_ADDITIONAL"
    make clean
    make
	echo "-------------------------Compile and Link Done--------------------------"

    cat $TMP_DIR/BBnames.txt | rev | cut -d: -f2- | rev | sort | uniq > $TMP_DIR/BBnames2.txt && mv $TMP_DIR/BBnames2.txt $TMP_DIR/BBnames.txt
    cat $TMP_DIR/BBcalls.txt | sort | uniq > $TMP_DIR/BBcalls2.txt && mv $TMP_DIR/BBcalls2.txt $TMP_DIR/BBcalls.txt
	
    $AFLGO/scripts/gen_distance_fast.py $Binary_DIR/examples/telnet-server/ $TMP_DIR telnet-server.minimal-net
	export COMPILE_ADDITIONAL="-distance=$TMP_DIR/distance.cfg.txt"
	export LINK_ADDITIONAL="-distance=$TMP_DIR/distance.cfg.txt"
	export CC="$AFLGO/afl-clang-fast $COMPILE_ADDITIONAL"
	export CXX="$AFLGO/afl-clang-fast++ $LINK_ADDITIONAL"
	make clean
	make

    cd $Build_dir
done < "$Targets_file"




