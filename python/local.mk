## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2013-2016 Vaucanson Group.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## The complete GNU General Public Licence Notice can be found as the
## `COPYING' file in the root directory.
##
## The Vaucanson Group consists of people listed in the `AUTHORS' file.

EXTRA_DIST += %D%/pylintrc

oodyn_gen = build-aux/bin/oodyn-gen
dist_noinst_python += $(oodyn_gen)
# Files generated by oodyn_gen.
from_oodyn_gen =				\
  %D%/oodyn.hh %D%/oodyn.cc
CLEANFILES += %D%/oodyn.stamp $(from_oodyn_gen)
%D%/oodyn.stamp: $(oodyn_gen) vcsn/dyn/algos.json
	$(AM_V_GEN)$(mkdir_p) $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(PYTHON) $(srcdir)/$(oodyn_gen)	\
	  --header %D%/oodyn.hh.tmp			\
	  --output %D%/oodyn.cc.tmp			\
	  vcsn/dyn/algos.json
	$(AM_V_at)for f in $(from_oodyn_gen);	\
	do						\
	  $(move_if_change) $$f.tmp $$f || exit 1;	\
	done
	$(AM_V_at)mv -f $@.tmp $@

$(from_oodyn_gen): %D%/oodyn.stamp
	@if test ! -f $@; then			\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi

BUILT_SOURCES += $(from_oodyn_gen)

vcsn_pythondir = $(pythondir)/vcsn
vcsn_python_PYTHON =                            \
  %D%/vcsn/__init__.py                          \
  %D%/vcsn/automaton.py                         \
  %D%/vcsn/context.py                           \
  %D%/vcsn/d3Widget.py                          \
  %D%/vcsn/dot.py                               \
  %D%/vcsn/expansion.py                         \
  %D%/vcsn/expression.py                        \
  %D%/vcsn/ipython.py                           \
  %D%/vcsn/label.py                             \
  %D%/vcsn/polynomial.py                        \
  %D%/vcsn/proxy.py                             \
  %D%/vcsn/python3.py                           \
  %D%/vcsn/score.py                             \
  %D%/vcsn/tools.py                             \
  %D%/vcsn/weight.py

vcsn_tools_pythondir = $(pythondir)/vcsn_tools
vcsn_tools_python_PYTHON =                      \
  %D%/vcsn_tools/demangle.py                    \
  %D%/vcsn_tools/gdb-demangle.py

vcsn_python_demodir = $(vcsn_pythondir)/demo
vcsn_python_demo_PYTHON =                       \
  %D%/vcsn/demo/__init__.py                     \
  %D%/vcsn/demo/automaton.py                    \
  %D%/vcsn/demo/eliminate_state.py

vcsn_python_pylint = %D%/pylintrc

pyexec_LTLIBRARIES = %D%/vcsn_cxx.la
%C%_vcsn_cxx_la_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_PYTHON_CPPFLAGS)
%C%_vcsn_cxx_la_LDFLAGS = -avoid-version -module $(BOOST_PYTHON_LDFLAGS)
%C%_vcsn_cxx_la_LIBADD = $(BOOST_PYTHON_LIBS) $(all_libctx) lib/libvcsn.la
%C%_vcsn_cxx_la_SOURCES = $(from_oodyn_gen) %D%/vcsn_cxx.cc

# Arguably should be taken care of by Automake.  But at least 1.15
# fails to clean the latter properly.
CLEANDIRS += %D%/__pycache__ %D%/vcsn_tools/__pycache__

# Before installing this library, wait for the libraries against which
# it is linked to be installed.  Otherwise libtool will complain that
# this Python library has dependencies that are not installed.
#
# However do not write "install-pyexecLTLIBRARIES: ...", because then Automake
# does not generate the rule!  We must obfuscate.
DASH = -
install$(DASH)pyexecLTLIBRARIES: install-pkglibLTLIBRARIES install-libLTLIBRARIES

# A simple means to express dependencies for Python tests.
VCSN_PYTHON_DEPS =				\
  $(HEADERS)					\
  $(nodist_python_PYTHON)			\
  $(nodist_vcsn_tools_python_PYTHON)		\
  $(pyexec_LTLIBRARIES)				\
  $(top_srcdir)/libexec/vcsn-compile		\
  $(vcsn_python_PYTHON)

# A short-hand to update all the python dependencies.
.PHONY: python
python: $(VCSN_PYTHON_DEPS)
