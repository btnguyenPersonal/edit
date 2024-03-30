#!/bin/bash

function testing() {
    if [ -z "$1" ]; then
        echo "Usage: $0 <argument>"
        return 1
    fi

    while true; do
        local ret=$(runner "$1")
        local status=$?
        
        echo "$ret"

        if [ $status -ne 0 ]; then
            break
        fi
    done
}

function runner() {
    build/e -c "$1"
    return $? 
}

testing "$1"
