#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./hcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0 
assert 42 42
assert 21 "5+20-4"
assert 21 " 5 + 20 - 4 "
assert 14 "2+3*4"
assert 4  "2*(6 - 4)"
assert 10 "((4 + 6) / 2) * (3 - 1)"
assert 6 "-5+3*-3+20"
assert 5 "-(-5)"


echo OK 