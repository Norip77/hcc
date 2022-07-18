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

assert 2 "func(x) {return x;} main(){return func(2);}"
assert 7 "func(x,y) {return x + y;} main(){return func(3,4);}"
assert 7 "func1(x,y){return func2(x,y,3);} func2(x,y,z) {return x+y*z;} main(){func1(1,2);}"
assert 13 "fib(x){if(x==2){return 1;} if(x == 1){return 1;}else{return fib(x-1) + fib(x-2);}} main(){fib(7);}"