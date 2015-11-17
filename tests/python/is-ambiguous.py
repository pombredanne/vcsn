#! /usr/bin/env python

import vcsn
from test import *

# Not deterministic, yet not ambiguous.
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab), b"
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> F
}
''')
CHECK(not a.is_ambiguous())
XFAIL(lambda: a.ambiguous_word(), "automaton is unambiguous")
CHECK(not a.is_deterministic())

# Not deterministic, and ambiguous.
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab), b"
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> F
  2 -> F
}
''')
CHECK(a.is_ambiguous())
CHECK_EQ("a", a.ambiguous_word())
CHECK(not a.is_deterministic())

# Test ambiguous word on automata with two ambiguous parts. Hence,
# test monomial_path with states different from pre and post.
b = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab), b"
  I -> 0
  I -> 1
  0 -> 2 [label = "a"]
  1 -> 2 [label = "a"]
  2 -> F
}
''')
CHECK(b.is_ambiguous())
CHECK((b*a).is_ambiguous())
CHECK((a*b).is_ambiguous())
CHECK_EQ("a", b.ambiguous_word())
CHECK_EQ("aa", (b*a).ambiguous_word())
CHECK_EQ("aa", (a*b).proper().ambiguous_word())

# Likewise, but with a non-commutative product.
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab), expressionset<lal_char(xy), b>"
  I -> 0
  0 -> 1 [label = "<x>a"]
  0 -> 2 [label = "<y>a"]
  1 -> F
  2 -> F
}
''')
CHECK(a.is_ambiguous())
CHECK_EQ("a", a.ambiguous_word())
CHECK(not a.is_deterministic())

# deterministic, and unambiguous (obviously).
a = vcsn.automaton('''
digraph
{
  vcsn_context="lal_char(ab), b"
  I -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> F
  2 -> F
}
''')
CHECK(not a.is_ambiguous())
XFAIL(lambda: a.ambiguous_word(), "automaton is unambiguous")
CHECK(a.is_deterministic())


## -------------------- ##
## is_cycle_ambiguous.  ##
## -------------------- ##

aut1 = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> 0 [label = "b"]
  1 -> 3 [label = "b"]
  2 -> 1 [label = "c"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 1 [label = "c"]
}''')
CHECK(aut1.is_cycle_ambiguous())

aut2 = vcsn.automaton('''digraph {
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "a"]
  1 -> 0 [label = "b"]
  1 -> 3 [label = "b"]
  2 -> 1 [label = "c"]
  2 -> 2 [label = "b"]
  3 -> F3
  3 -> 1 [label = "b"]
}''')
CHECK(not aut2.is_cycle_ambiguous())

aut3 = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(abc), b"
  I0 -> 0
  0 -> 1 [label = "c"]
  1 -> 2 [label = "a"]
  2 -> 3 [label = "b"]
  2 -> 4 [label = "b"]
  3 -> 5 [label = "c"]
  3 -> 6 [label = "c"]
  4 -> 7 [label = "c"]
  5 -> 2 [label = "a"]
  6 -> 0 [label = "b"]
  7 -> F7
  7 -> 2 [label = "a"]
}''')
CHECK(aut3.is_cycle_ambiguous())

r1 = "((abc)*){5}abc" + format(aut1.expression())
aut4 = vcsn.context("lal_char(abc), b").expression(r1).derived_term()
CHECK(aut4.is_cycle_ambiguous())

r2 = "((abc)*){5}abc" + format(aut2.expression())
aut5 = vcsn.context("lal_char(abc), b").expression(r2).derived_term()
CHECK(not aut5.is_cycle_ambiguous())


CHECK(vcsn.context("lal_char(abc), b").
         ladybird(5).is_cycle_ambiguous())
CHECK(not vcsn.context("lal_char(abc), b").
         de_bruijn(5).is_cycle_ambiguous())
CHECK(vcsn.context("lal_char(abc), b").
         ladybird(20).is_cycle_ambiguous())
CHECK(not vcsn.context("lal_char(abc), b").
        de_bruijn(20).is_cycle_ambiguous())
