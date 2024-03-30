#!/bin/bash

function testing() {
    local chars=($'\x00' $'\x01' $'\x02' $'\x03' $'\x05' $'\x06' $'\x07' \
                 $'\x08' $'\x09' $'\x0A' $'\x0B' $'\x0C' $'\x0D' $'\x0E' $'\x0F' \
                 $'\x10' $'\x11' $'\x12' $'\x13' $'\x14' $'\x16' $'\x17' \
                 $'\x18' $'\x19' $'\x1A' $'\x1B' $'\x1C' $'\x1D' $'\x1E' $'\x1F' \
                 ' ' '!' '"' '#' '$' '%' '&' "'" '(' ')' '*' '+' ',' '-' '.' '/' \
                 '0' '1' '2' '3' '4' '5' '6' '7' '8' '9' ':' ';' '<' '=' '>' '?' \
                 '@' 'A' 'B' 'C' 'D' 'E' 'F' 'G' 'H' 'I' 'J' 'K' 'L' 'M' 'N' 'O' \
                 'P' 'Q' 'R' 'S' 'T' 'U' 'V' 'W' 'X' 'Y' 'Z' '[' '\\' ']' '^' '_' \
                 '`' 'a' 'b' 'c' 'd' 'e' 'f' 'g' 'h' 'i' 'j' 'k' 'l' 'm' 'n' 'o' \
                 'p' 'q' 'r' 's' 't' 'u' 'v' 'w' 'x' 'y' 'z' '{' '|' '}' '~')
    generate_combinations() {
        local prefix="$1"
        local length="$2"

        if [ "$length" -le 0 ]; then
            local printable=$(echo -n "$prefix" | od -An -t x1 | tr -d '\n' | sed 's/ /\\x/g')
            echo "'$printable'"
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
