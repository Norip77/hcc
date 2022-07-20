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

assert 3 "int main(){return 3;}"
assert 3 "int main(int x){return 3;}"
assert 4 "int main(){int x;int y;int z;x = 1;y = 3; return x+y;}"
assert 3 "int func(){return 3;} int main(){int x; x = 2;return func();}"
assert 2 "int main(){int x; int y; x = 1; y = 1;func(); return x+y;} int func(int x){return x;}"

assert 2 "int func(int x) {return x;} int main(){return func(2);}"
assert 7 "int func(int x,int y) {return x + y;} int main(){return func(3, 4);}"
assert 7 "int func1(int x,int y){return func2(x,y,3);} int func2(int x,int y,int z) {return x+y*z;} int main(){func1(1,2);}"
assert 13 "int fib(int x){if(x==2){return 1;} if(x == 1){return 1;}else{return fib(x-1) + fib(x-2);}} int main(){fib(7);}"

assert 3 "int main(){int x; int *y; y = &x; *y = 3; return x;}"
assert 3 "int main(){int x; int *y; int **z; y = &x; z = &y; **z = 3; return x;}"