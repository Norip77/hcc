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

assert 0 "0;" 
assert 42 "42;"
assert 21 "5+20-4;"
assert 21 " 5 + 20 - 4; "
assert 14 "2+3*4;"
assert 4  "2*(6 - 4);"
assert 10 "((4 + 6) / 2) * (3 - 1);"
assert 6 "-5+3*-3+20;"
assert 5 "-(-5);"

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '2>=0;'
assert 1 '2>=2;'
assert 0 '2>=3;'

assert 3 "10; 20;3;"
assert 9 "1+2; 3-4; 5*2-1;"
assert 1 "a=1;"
assert 5 "d = 5;"
assert 30 "a = 10; b = 20; c = 30;"
assert 32 "x = 4; y = 9; z = x * (y - 1);"

assert 3 "pi = 3;"
assert 150 "height = 10; width = 15; area = height * width;"

assert 3 "return 3;"
assert 4 "return 4; return 5;"
assert 7 "2*3; return_x = 7; return return_x;"
assert 2 "return(1+1);"

assert 3 "if(1) return 3;"
assert 5 "abc = 3; if(abc == 3) return 5;"
assert 5 "abc = 3; if(abc < 9) return 5;"
assert 10 "abc = 3; if(abc > 9) return 5; return 10;"

assert 7 "if(1 == 2) return 1; else return 7;"
assert 1 "age = 22; if(age >= 20) candrink = 1; else candrink = 0; return candrink;"

assert 3 "while(0) 1; return 3;"
assert 0 "a = 5; while(a = a - 1) 1; return a;"
assert 45 "a = 10;b = 0; while(a = a - 1) b = b + a;"
assert 3 "abc = 10; while(abc = abc - 1) if(abc == 3) return abc;"

assert 45 "b = 0; for(a = 0;a < 10;a = a + 1) b = b + a; return b;"
assert 16 "a = 8; if((a = a * 2) > 10) return a;"
assert 16 "a = 2; for(;;) if((a = a * 2) > 10) return a;"

assert 1 "if(1 == 1){return 1;}"
assert 5 "for(i = 0; i < 10;){i = i + 1; if(i == 5){return i;}}"

assert 3 "x = 3; y = &x; return *y;"
assert 3 "x = 3; y = 5; z = &y + 8; return *z;"
echo OK 