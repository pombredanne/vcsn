#! /usr/bin/env python

import vcsn
from test import *

ctx = vcsn.context('lal_char(abcd), z')
def expr(e, ids='associative'):
    return ctx.expression(e, ids)

# check(INPUT, EXP)
# -----------------
def check(input, exp=None):
    if exp is None:
        exp = input
    ref = expr(exp)
    CHECK_EQ(ref, expr(input).expand())

check('\z')
check('\e')
check('a')
check('<2>a')
check('a+a', '<2>a')
check('b+a+b+a', '<2>a+<2>b')

check('(a*)&(b+a+b+a)', '<2>(a*&a)+<2>(a*&b)')

check('<3>(b+a+b+a)<5>', '<30>a+<30>b')

check('(a+b)?{3}', \
      r'\e+<3>a+<3>b+<3>(aa)+<3>(ab)+<3>(ba)+<3>(bb)' \
      r'+aaa+aab+aba+abb+baa+bab+bba+bbb')

# TAF-Kit doc
check('(a+b+\e)((a+ba)(ca+cc))*',\
      '(aca+acc+baca+bacc)*+a(aca+acc+baca+bacc)*+b(aca+acc+baca+bacc)*')

check('a(b(c+a)*+c(b)*)+ac(\e+b)(b*)', '<2>(acb*)+ab(a+c)*+acbb*')
