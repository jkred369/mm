#!/bin/bash
# start a server
# assuming root folder for MM is defined as MM_PATH in env variable

export MM_LIB_PATH=$MM_PATH/lib:$LD_LIBRARY_PATH

$MM_PATH/bin/$1 $MM_PATH/config/$2
