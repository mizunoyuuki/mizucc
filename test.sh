#!/bin/bash
assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expexted, but got $actual"
		exit 1
	fi
}

assert 0 '0;'
assert 42 '42;'
assert 42 '21+21-10+10;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 4 '-15+(-3+3)+19;'
assert 32 '+4*+8;'
assert 0 '0==1;'
assert 1 '12==12;'
assert 14 '(12==12)+13;'
assert 1 '0<9;'
assert 0 '12<10;'
assert 0 '12>12;'
assert 1 '12>=12;'
assert 0 '12   <  11;'
assert 2 '12-12;13-11;'
assert 11 'a=12;a-1;'
assert 12 'a=15;b=a-3;b;'
assert 10 'a = 11; b = 1; a-b;'
assert 10 'abc = 23; def = 10; def = 13; abc - def;'
assert 23 'abc = 23; def = 10; def = 13; abc;'
assert 13 'abc = 23; def = 10; def = 13; def;'
assert 14 '
abc = 23;
def = 10;
def = 234 - 220;
'

assert 19 '
abc = 19;
name = 111;
return abc;
return name;
'

echo OK
