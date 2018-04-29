#! /usr/bin/env python

# Check parsing and pretty-printing.

import vcsn
from test import *

## ------------- ##
## Format: ERE.  ##
## ------------- ##

ctx = vcsn.context("[a-z...] -> b")

XFAIL(lambda: ctx.expression('a', format='foo'),
      '''invalid rational expression format: foo, expected: default, ere, grep, text, vcsn
  while reading expression: "a"''')

def check(e, exp = None, ere = None, redgrep = None):
    '''Check that `e` parsed in ere is `exp` (defaults to `e`) in Vcsn
    format, and `ere` in ERE format (defaults to `e` itself).

    '''
    print("Check: {}".format(e))
    if exp is None:
        exp = e
    if ere is None:
        ere = e
    if redgrep is None:
        redgrep = ere
    e = ctx.expression(e, format='ere')
    CHECK_EQ(exp, e.format('text'))
    CHECK_EQ(ere, e.format('ere'))
    CHECK_EQ(redgrep, e.format('redgrep'))
def xfail(e, err, fmt=['ere']):
    for f in fmt:
        XFAIL(lambda: ctx.expression(e, format=f), err)

check('', r'\e', '()')
check('()', r'\e')
check('a')
check('ab')
check('a+', 'aa*', 'aa*')
check('a|b', 'a+b')
check('[ab]', 'a+b', 'a|b')
check('[abcd]', '[a-d]', '[a-d]')
check('[a-d]')
check('[^a-d]')
check('!a',  r'\!a',  redgrep=r'\!a')
check('a&b', r'a\&b', redgrep=r'a\&b')
check('.', '[^]', '.')

check('a?', r'\e+a', 'a?')
# This one is tricky: `*` needs need issue parens, although the real nature of
# the child is a sum, which does require parens (as in `(\e+a)*`).
check('a?*', r'(\e+a)*')
check('(ab)?', r'\e+ab')
check('(ab)*?', r'\e+(ab)*')
check('(ab)?*', r'(\e+ab)*')
# A sum partly printed as a label class.
check('([a-d]|abcd)?', r'\e+[a-d]+abcd')

xfail('^a', '''1.1: unsupported operator: ^
^a
^
  while reading expression: "^a"''')
xfail('a$', '''1.2: unsupported operator: $
a$
 ^
  while reading expression: "a$"''')
check(r'\^\$', '^$')
xfail(r'\b', r'''1.1-2: unsupported operator: \b
\b
^^
  while reading expression: "\\b"''')
check(r'\^\$', '^$')

## --------- ##
## Escapes.  ##
## --------- ##

ctx = vcsn.context("[...] -> b")

def check(n):
    e = r'\x{:02x}'.format(n)
    print(e)
    e = ctx.expression(e)
    CHECK_EQ(e, ctx.expression(str(e)))

# We use 0 for \e and -1 for $.
for i in range(1, 255):
    check(i)

XFAIL(lambda: ctx.expression(r'\xff'),
      r'add_letter: the special letter is reserved: \xff')


## ------------- ##
## Copy/convert. ##
## ------------- ##

def check(e, ctx=None, ids=None, exp=None):
    '''When `e` is converted to `ctx` and `ids`, it should be `exp`.
    `exp` defaults to `e`.'''
    if ctx is None:
        ctx = e.context()
    if ids is None:
        ids = e.identities()
    if exp is None:
        exp = e
    if not isinstance(ctx, vcsn.context):
        ctx = vcsn.context(ctx)
    CHECK_EQ(exp, e.expression(ctx, ids))

q = vcsn.context('[...] -> q')
qexp = q.expression

e = qexp('a+b+<1/2>a', 'none')
check(e, q, 'none', e)
check(e, q, 'linear', '⟨3/2⟩a+b')
check(e, '[...]* -> q', 'none', '(a+b)+⟨1/2⟩a')
check(e, '[...] -> r', 'none', '(a+b)+⟨0.5⟩a')
check(e, '[...]*, r', 'linear', '⟨1.5⟩a+b')

check(qexp(r'\z*', 'none'), q, 'none', r'∅*')
check(qexp(r'\z*', 'none'), q, 'linear', r'ε')

check(qexp(r'\z'))
check(qexp(r'\e'))

check(qexp(r'ab'))
check(qexp(r'a+b'))
check(qexp(r'a&b'))
check(qexp(r'a:b'))
check(qexp(r'a&:b'))
check(qexp(r'a{\}b'))

check(qexp(r'a*'))
check(qexp(r'a{c}'))
check(qexp(r'a{T}'))
# Doesn't make any sense, but that's not for copy to check that.
check(qexp(r'a@b'))

q3 = q ** 3
check(qexp('a'),    q3, exp='a|a|a')
check(qexp('ab*c'), q3, exp='(a|a|a)(b|b|b)*(c|c|c)')
check(qexp('abc'),  q3, exp='(a|a|a)(b|b|b)(c|c|c)')


## ------------ ##
## Complement.  ##
## ------------ ##

ctx = vcsn.context("[abcd] -> b")

def check_complement(r1):
    '''Check that `complement` on a rational expression corresponds to
    its concrete syntax.'''
    eff = ctx.expression(r1).complement()
    exp = ctx.expression('({}){{c}}'.format(r1))
    CHECK_EQ(exp, eff)

check_complement(r'\z')
check_complement('ab')

## ---------- ##
## Multiply.  ##
## ---------- ##
def check_concat(r1, r2):
    '''Check that `*` between rational expression corresponds to
    concatenation concrete syntax.'''
    eff = ctx.expression(r1) * ctx.expression(r2)
    exp = ctx.expression('({})({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_concat('ab', 'cd')
check_concat('a', 'bcd')
check_concat('abab', 'bbbb')
check_concat('a*', 'a*b*')
check_concat('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_concat('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_concat('a', r'\e')
check_concat('a', r'\z')

## ------------ ##
## Difference.  ##
## ------------ ##
def check_difference(r1, r2):
    '''Check that `%` between rational expression corresponds to
    `%` in concrete syntax.'''
    eff = ctx.expression(r1) % ctx.expression(r2)
    exp = ctx.expression('({})%({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_difference('ab', 'cd')
check_difference('a', 'bcd')
check_difference('abab', 'bbbb')
check_difference('a*', 'a*b*')
check_difference('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_difference('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_difference('a', r'\e')
check_difference('a', r'\z')

## ------------- ##
## Conjunction.  ##
## ------------- ##
def check_conj(r1, r2):
    '''Check that `&` between rational expression corresponds to
    `&` in concrete syntax.'''
    eff = ctx.expression(r1) & ctx.expression(r2)
    exp = ctx.expression('({})&({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_conj('ab', 'cd')
check_conj('a', 'bcd')
check_conj('abab', 'bbbb')
check_conj('a*', 'a*b*')
check_conj('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_conj('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_conj('a', r'\e')
check_conj('a', r'\z')

## ----- ##
## Add.  ##
## ----- ##
def check_sum(r1, r2):
    '''Check that `+` between rational expression corresponds to
    `+` in concrete syntax.'''
    eff = ctx.expression(r1) + ctx.expression(r2)
    exp = ctx.expression('({})+({})'.format(r1, r2))
    CHECK_EQ(exp, eff)

check_sum('ab', 'cd')
check_sum('a', 'bcd')
check_sum('abab', 'bbbb')
check_sum('a*', 'a*b*')
check_sum('a*+b*+c+c*', '(a*+b*+c+c*)*')
check_sum('(a*+b*+c+c*)*', '(a*a*a*b*b*a+b+a+b+a)')
check_sum('a', r'\e')
check_sum('a', r'\z')


## --------------- ##
## Transposition.  ##
## --------------- ##
def check_transposition(r1):
    '''Check that `transposition` on a rational expression corresponds to
    its concrete syntax.'''
    eff = ctx.expression(r1).transposition()
    exp = ctx.expression('({}){{T}}'.format(r1))
    CHECK_EQ(exp, eff)

check_transposition(r'\z')
check_transposition('ab')


## --------------------- ##
## Invalid expressions.  ##
## --------------------- ##

# Check invalid input.
def xfail(r):
    XFAIL(lambda: ctx.expression(r))

ctx = vcsn.context('[abc] -> b')
xfail('')
xfail('<2>a')
xfail('x')
xfail('a+')
xfail('a(')
xfail('a[')
xfail('*')
xfail('&a')
xfail('a&')
xfail(r'\a')

ctx = vcsn.context('[abc] -> q')
xfail('(?@[abc] -> b)<2>a')
xfail('(?@[abc] -> b')
xfail('(?@foobar)foo')
xfail('<2>')
xfail('a<2')


## -------- ##
## format.  ##
## -------- ##

# FIXME: This should be part of check-rat.
def check_format(ctx, r, **kwargs):
    ctx = vcsn.context(ctx)
    e = ctx.expression(r)
    for fmt, exp in kwargs.items():
        CHECK_EQ(exp, e.format(fmt))

check_format('[abcd] -> b',
             'abcd',
             text='abcd',
             utf8='abcd',
             latex=r'a \, b \, c \, d')

# Check classes.  FIXME: Redundant with check-rat.
check_format('[abcdef] -> b',
             '[abcdef]',
             text='[^]', utf8='[^]', latex=r'[\hat{}]')
check_format('[abcdef] -> b',
             '[abcde]',
             text='[^f]', utf8='[^f]', latex=r'[\hat{}f]')
check_format('[a-z] -> b',
             '[abcd]',
             text='[a-d]', utf8='[a-d]', latex=r'[a\textrm{-}d]')

# Check weights.
check_format('[abc] -> RatE[[def] -> RatE[[xyz] -> q]]',
             '<<<42>x>d>a+<<<51>x>d>a+(<<<42>y>e>b)*',
             text='<<<93>x>d>a+(<<<42>y>e>b)*',
             utf8='⟨⟨⟨93⟩x⟩d⟩a+(⟨⟨⟨42⟩y⟩e⟩b)*',
             latex=r' \left\langle  \left\langle  \left\langle 93 \right\rangle \,x \right\rangle \,d \right\rangle \,a + \left( \left\langle  \left\langle  \left\langle 42 \right\rangle \,y \right\rangle \,e \right\rangle \,b\right)^{*}')

# Words are in \mathit to get correct inter-letter spacing.
check_format('[abc]* -> q',
             '(abc)a(bc)',
             text='(abc)a(bc)',
             utf8='(abc)a(bc)',
             latex=r'\left(\mathit{abc}\right) \, \mathit{a} \, \left(\mathit{bc}\right)')

# Exponents.
for ctx in ['[...] -> Q', '[...]* -> Q']:
    # In wordset, for LaTeX, we use \mathit.
    A = '\mathit{a}' if ctx == '[...]* -> Q' else 'a'
    check_format(ctx,
                 'a{2}',
                 text='aa', utf8='aa', latex=r'a \, a'.replace('a', A))
    check_format(ctx,
                 'a{3}',
                 text='aaa', utf8='a³', latex=r'{a}^{3}'.replace('a', A))
    check_format(ctx,
                 'a{4}',
                 text='aaaa', utf8='a⁴', latex=r'{a}^{4}'.replace('a', A))
    check_format(ctx,
                 'a{5}',
                 text='a{5}', utf8='a⁵', latex=r'{a}^{5}'.replace('a', A))
    check_format(ctx,
                 'a{10}',
                 text='a{10}', utf8='a¹⁰', latex=r'{a}^{10}'.replace('a', A))
    check_format(ctx,
                 'a{100}',
                 text='a{100}', utf8='a¹⁰⁰', latex=r'{a}^{100}'.replace('a', A))
    check_format(ctx,
                 'a{987}',
                 text='a{987}', utf8='a⁹⁸⁷', latex=r'{a}^{987}'.replace('a', A))

# Check that we do support digits as letters.
check_format('[0123] -> b',
             '0123',
             text='0123',
             utf8='0123',
             latex='0 \\, 1 \\, 2 \\, 3')
check_format('[0123] -> q',
             '<0123>0123',
             text='<123>(0123)',
             utf8='⟨123⟩(0123)',
             latex=r' \left\langle 123 \right\rangle \,\left(0 \, 1 \, 2 \, 3\right)')

## ------------------------- ##
## Distributive identities.  ##
## ------------------------- ##
def check(s1, exp):
    eff = ctx.expression(s1, 'distributive')
    CHECK_EQ(exp, eff.format('text'))

ctx = vcsn.context('[abcd] -> q')
check('a+b', 'a+b')
check('(a+a)*', '(<2>a)*')
check(r'a+\z', 'a')
check('(<5>a)b', '<5>(ab)')
check('(<5>a)(b)(c*)', '<5>(abc*)')
check('a+b(c+<2>d)', 'a+bc+<2>(bd)')
check('a*+b(c+<2>d)', 'a*+bc+<2>(bd)')

ctx = vcsn.context('[abcd]* -> q')
check('(a<5>)b', '<5>(ab)')
check('a+b(c+<2>d)', 'a+bc+<2>(bd)')
check('a*+b(c+<2>d)', 'a*+bc+<2>(bd)')


## ------------- ##
## Dot output.   ##
## ------------- ##

def check(exp, dot, identities=['trivial', 'associative']):
    for ids in identities:
        e = ctx.expression(exp, ids)
        for type in ['logical', 'physical']:
            fname = '{}-{}-{}.gv'.format(dot, ids, type)
            print("Checking dot output:", fname)
            ref = metext(fname)
            CHECK_EQ(ref, e.dot(type == 'physical'))

ctx = vcsn.context('[...] -> Q')
check('(<2>[abc])*a([abc]<3>){3}',
      'de-bruijn')
check('\e{2}+\z{2}+a{2}',
      'labels', ['none'])


ctx = vcsn.context('[...] x [...] -> q')
check('([ab]{3}|x* + [cd]{2}|y*){2}',
      'lan-lan')
