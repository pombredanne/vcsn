#! /usr/bin/env python

import vcsn
from test import *

# Precondition: lal.
XFAIL(lambda: vcsn.automaton('''
digraph
{
  vcsn_context = "law_char(ab)_b"
  I -> 0
  0 -> 0 [label="a, b"]
  0 -> F
}
''').complement())

# Precondition: Boolean.
XFAIL(lambda: vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_z"
  I -> 0
  0 -> 0 [label="a, b"]
  0 -> F
}
''').complement())

# Precondition: deterministic.
XFAIL(lambda: vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  I -> 0
  0 -> 1 [label="a"]
  0 -> 2 [label="a,b"]
  2 -> 2 [label="a,b"]
  2 -> F
}
''').complement())

# Precondition: complete.
XFAIL(lambda: vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  I -> 0
  0 -> 0 [label="a"]
  0 -> F
}
''').complement())

# Complement for real.
a = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F1
  }
  {
    node [shape = circle]
    0
    1
    2 [color = DimGray]
  }
  I0 -> 0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b", color = DimGray]
  1 -> F1
  1 -> 2 [label = "a, b", color = DimGray]
  2 -> 2 [label = "a, b", color = DimGray]
}
''')

o = vcsn.automaton('''
digraph
{
  vcsn_context = "lal_char(ab)_b"
  rankdir = LR
  {
    node [style = invis, shape = none, label = "", width = 0, height = 0]
    I0
    F0
    F2
  }
  {
    node [shape = circle]
    0
    1
    2
  }
  I0 -> 0
  0 -> F0
  0 -> 1 [label = "a"]
  0 -> 2 [label = "b"]
  1 -> 2 [label = "a, b"]
  2 -> F2
  2 -> 2 [label = "a, b"]
}
''')
CHECK_EQ(True, a.complement().is_equivalent(o))

# Involution.
CHECK_EQ(True, a.complement().complement().is_equivalent(a))
