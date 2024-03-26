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

export AFLGO=$LTLFuzzer"AFLGo/"
export CC=$AFLGO"afl-clang-fast"
export CXX=$AFLGO"afl-clang-fast++"

INC=$LTLFuzzer"include"
INST_LIB=$LTLFuzzer"build/src/instrumentation/libinstrumentation.a"
ATM_LIB=$LTLFuzzer"build/src/automata/libautomata.a"


cd $Build_dir
while IFS=":" read -r fileName lineNum event
do
    if [ -d  $fileName":"$lineNum ]; then
        rm -rf $fileName":"$lineNum
    fi
    mkdir $fileName":"$lineNum
    Binary_DIR=$(realpath $fileName":"$lineNum)
    cd $Binary_DIR && mkdir TMP
    TMP_DIR=$(realpath TMP)
    cp $Prj_dir/* .

    echo $fileName":"$lineNum >$TMP_DIR/BBtargets.txt

    export ADDITIONAL="-targets=$TMP_DIR/BBtargets.txt -outdir=$TMP_DIR -flto  -fuse-ld=gold -Wl,-plugin-opt=save-temps"

    $CXX $ADDITIONAL -o Problem1 Problem1.c -I $INC $INST_LIB $ATM_LIB -lrt -lspot -lpthread -lbddx

    cat $TMP_DIR/BBnames.txt | rev | cut -d: -f2- | rev | sort | uniq > $TMP_DIR/BBnames2.txt && mv $TMP_DIR/BBnames2.txt $TMP_DIR/BBnames.txt
    cat $TMP_DIR/BBcalls.txt | sort | uniq > $TMP_DIR/BBcalls2.txt && mv $TMP_DIR/BBcalls2.txt $TMP_DIR/BBcalls.txt
    $AFLGO/scripts/gen_distance_fast.py $Binary_DIR $TMP_DIR Problem1
    $CXX -distance=$TMP_DIR/distance.cfg.txt -revents=$Targets_file -o Problem1  Problem1.c -I $INC $INST_LIB $ATM_LIB -lrt -lspot -lpthread -lbddx

    cp $TMP_DIR/distance.cfg.txt .

    cd $Build_dir
done < "$Targets_file"
