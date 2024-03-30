#!/bin/bash

function testing() {
    local chars=(        	 
                 {a..z} {A..Z} {0..9})
    generate_combinations() {
        local prefix="$1"
        local length="$2"

        if [ "$length" -le 0 ]; then
            echo "$prefix"
            runner "$prefix"
            local status=$?

            if [ $status -ne 0 ]; then
                local printable=$(echo -n "$prefix" | od -An -t x1 | tr -d '\n' | sed 's/ /\\x/g')
                echo "Runner failed on string: '$printable'"
                exit
            fi
        else
            for char in "${chars[@]}"; do
                generate_combinations "$prefix$char" $(($length - 1))
            done
        fi
    }
    for length in {1..10}; do
        generate_combinations "" $length
    done
}

function runner() {
    build/e longtest.md -c "$1"
    return $? 
}

testing "$1"
