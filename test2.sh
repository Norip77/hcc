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

assert 3 "main(){return 3;}"
assert 4 "main(){x = 1; y = 3; return x+y;}"
assert 3 "func(){return 3;} main(){return func();}"