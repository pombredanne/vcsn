#! /usr/bin/env python

import vcsn
from test import *

z = vcsn.context('law_char(abcd), z')
zp = z.polynomial

## ------------------------------ ##
## ldiv(polynomial, polynomial).  ##
## ------------------------------ ##

def check(exp, lhs, rhs):
    exp = zp(exp)
    lhs = zp(lhs)
    rhs = zp(rhs)
    CHECK_EQ(exp, lhs.ldiv(rhs))

def xfail(lhs, rhs):
    lhs = zp(lhs)
    rhs = zp(rhs)
    XFAIL(lambda: lhs.ldiv(rhs))

xfail('a', 'b')

check(r'\e', 'a', 'a')
check(r'\e', '<2>a', '<2>a')
check(r'<2>\e', '<2>a', '<4>a')

xfail('a', 'a+b')
check(r'\e', 'a+b', 'a+b')
xfail('a+b', 'a+<2>b')
check(r'\e', 'a+<2>b', 'a+<2>b')

xfail('ab', 'ac')
check(r'c+d', 'ab', 'abc+abd')
check(r'<2>c+<3>d', 'ab', '<2>abc+<3>abd')

check(r'<2>c+<3>d', '<2>ab', '<4>abc+<6>abd')

xfail('a+aa', 'a')

## ------------------------------ ##
## lgcd(polynomial, polynomial).  ##
## ------------------------------ ##

def check(exp, lhs, rhs):
    exp = zp(exp)
    lhs = zp(lhs)
    rhs = zp(rhs)
    CHECK_EQ(exp, lhs.lgcd(rhs))

check('a', 'a', 'a')
check('\e', 'a', 'b')

check('<2>a', '<2>a', '<2>a')
check('<2>a', '<2>a', '<4>a')
check('a',    '<2>a', '<3>a')

check('ab', 'ab', 'ab')
#check('a', 'ab', 'ac')

#check('<2>a', '<4>ab', '<6>ac')

#check('<2>a+<3>b', '<2>a+<3>b', '<4>ac+<6>bc')
#check('<2>a+<3>b', '<6>a+<9>b', '<4>ac+<6>bc')

check('\e', 'a+b', 'a+c')