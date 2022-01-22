#!/bin/bash

BUILD_DIR=$1;
FORMULA=$2;

cd $BUILD_DIR
echo "$FORMULA" > ltl.txt
