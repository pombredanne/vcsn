#! /usr/bin/env python

import vcsn

from test import *

# check OBJ EXPECTED
# ------------------
# Check that OBJ.info(True) == EXPECTED.
def check(obj, exp):
    CHECK_EQ(exp, obj.info(True))

## ---------------- ##
## automaton.info.  ##
## ---------------- ##

# Check that ':', which is used as a separator by info, is also
# properly treated as a letter.
check(vcsn.context('lal_char(:a-z), z').expression("a+a':'").standard(),
      {'is ambiguous': False,
       'is codeterministic': False,
       'is complete': False,
       'is cycle ambiguous': False,
       'is deterministic': False,
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is synchronizing': False,
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 4,
       'number of coaccessible states': 4,
       'number of codeterministic states': 4,
       'number of deterministic states': 3,
       'number of eps transitions': 0,
       'number of final states': 2,
       'number of initial states': 1,
       'number of states': 4,
       'number of strongly connected components': 4,
       'number of transitions': 3,
       'number of useful states': 4,
       'type': 'mutable_automaton<letterset<char_letters(:abcdefghijklmnopqrstuvwxyz)>, z>'})

check(vcsn.context('lal_char(a-z), z').expression("<1>a+(<2>b<3>*<4>)<5>").standard(),
      {'is ambiguous': False,
       'is codeterministic': False,
       'is complete': False,
       'is cycle ambiguous': False,
       'is deterministic': True,
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is synchronizing': False,
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 3,
       'number of coaccessible states': 3,
       'number of codeterministic states': 2,
       'number of deterministic states': 3,
       'number of eps transitions': 0,
       'number of final states': 3,
       'number of initial states': 1,
       'number of states': 3,
       'number of strongly connected components': 3,
       'number of transitions': 3,
       'number of useful states': 3,
       'type': 'mutable_automaton<letterset<char_letters(abcdefghijklmnopqrstuvwxyz)>, z>'})

# Test what happens with "N/A".
check(vcsn.context('law_char(ab), b').expression('a(a+b)*').standard(),
      {'is ambiguous': 'N/A',
       'is codeterministic': 'N/A',
       'is complete': 'N/A',
       'is cycle ambiguous': 'N/A',
       'is deterministic': 'N/A',
       'is empty': False,
       'is eps-acyclic': True,
       'is normalized': False,
       'is proper': True,
       'is standard': True,
       'is synchronizing': 'N/A',
       'is trim': True,
       'is useless': False,
       'is valid': True,
       'number of accessible states': 4,
       'number of coaccessible states': 4,
       'number of deterministic states': 'N/A',
       'number of codeterministic states': 'N/A',
       'number of eps transitions': 0,
       'number of final states': 3,
       'number of initial states': 1,
       'number of states': 4,
       'number of strongly connected components': 3,
       'number of transitions': 7,
       'number of useful states': 4,
       'type': 'mutable_automaton<wordset<char_letters(ab)>, b>'})


## ------------- ##
## expression.info.  ##
## ------------- ##

check(vcsn.context('lal_char(abc), b').expression('abc'),
      {'type': 'expressionset<letterset<char_letters(abc)>, b>',
       'size': 5,
       'sum': 0,
       'shuffle': 0,
       'conjunction': 0,
       'prod': 1,
       'star': 0,
       'complement': 0,
       'zero': 0,
       'one': 0,
       'atom': 3,
       'lweight': 0,
       'rweight': 0})

check(vcsn.context('lal_char(abc), b').expression('\e+bc*'),
      {'type': 'expressionset<letterset<char_letters(abc)>, b>',
       'size': 6,
       'sum': 1,
       'shuffle': 0,
       'conjunction': 0,
       'prod': 1,
       'star': 1,
       'complement': 0,
       'zero': 0,
       'one': 1,
       'atom': 2,
       'lweight': 0,
       'rweight': 0})

check(vcsn.context('lal_char(abc), z').expression('<2>a<3>'),
      {'type': 'expressionset<letterset<char_letters(abc)>, z>',
       'size': 2,
       'sum': 0,
       'shuffle': 0,
       'conjunction': 0,
       'prod': 0,
       'star': 0,
       'complement': 0,
       'zero': 0,
       'one': 0,
       'atom': 1,
       'lweight': 1,
       'rweight': 0})

check(vcsn.context('lal_char(abc), z').expression('<2>(a+b)<3>'),
      {'type': 'expressionset<letterset<char_letters(abc)>, z>',
       'size': 5,
       'sum': 1,
       'shuffle': 0,
       'conjunction': 0,
       'prod': 0,
       'star': 0,
       'complement': 0,
       'zero': 0,
       'one': 0,
       'atom': 2,
       'lweight': 1,
       'rweight': 1})
