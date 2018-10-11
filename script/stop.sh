#!/bin/bash
# stop a server
# assuming root folder for MM is defined as MM_PATH in env variable

export MM_LIB_PATH=$MM_PATH/lib:$LD_LIBRARY_PATH

MM_PID=ps -ef | grep $1 | awk '{print $2}'

kill -10 $MM_PID
