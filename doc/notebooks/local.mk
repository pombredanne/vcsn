## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2014-2015 Vaucanson Group.
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

notebooksdir = $(htmldir)/notebooks

dist_notebooks_DATA =                           \
  %D%/Algorithms.ipynb                          \
  %D%/Automata.ipynb                            \
  %D%/Contexts.ipynb                            \
  %D%/D3_IPython_widget.ipynb                   \
  %D%/Expressions.ipynb                         \
  %D%/Glossary.ipynb                            \
  %D%/Hacking.ipynb                             \
  %D%/References.ipynb                          \
  %D%/Stackoverflow.ipynb                       \
  %D%/Transducers.ipynb                         \
  %D%/automaton.accessible.ipynb                \
  %D%/automaton.ambiguous_word.ipynb            \
  %D%/automaton.coaccessible.ipynb              \
  %D%/automaton.codeterminize.ipynb             \
  %D%/automaton.cominimize.ipynb                \
  %D%/automaton.complement.ipynb                \
  %D%/automaton.complete.ipynb                  \
  %D%/automaton.compose.ipynb                   \
  %D%/automaton.conjunction.ipynb               \
  %D%/automaton.costandard.ipynb                \
  %D%/automaton.delay_automaton.ipynb           \
  %D%/automaton.determinize.ipynb               \
  %D%/automaton.eliminate_state.ipynb           \
  %D%/automaton.expression.ipynb                \
  %D%/automaton.filter.ipynb                    \
  %D%/automaton.has_bounded_lag.ipynb           \
  %D%/automaton.has_twins_property.ipynb        \
  %D%/automaton.infiltration.ipynb              \
  %D%/automaton.is_ambiguous.ipynb              \
  %D%/automaton.is_codeterministic.ipynb        \
  %D%/automaton.is_complete.ipynb               \
  %D%/automaton.is_cycle_ambiguous.ipynb        \
  %D%/automaton.is_deterministic.ipynb          \
  %D%/automaton.is_empty.ipynb                  \
  %D%/automaton.is_equivalent.ipynb             \
  %D%/automaton.is_functional.ipynb             \
  %D%/automaton.is_isomorphic.ipynb             \
  %D%/automaton.is_partial_identity.ipynb       \
  %D%/automaton.is_standard.ipynb               \
  %D%/automaton.is_synchronized.ipynb           \
  %D%/automaton.is_useless.ipynb                \
  %D%/automaton.is_valid.ipynb                  \
  %D%/automaton.letterize.ipynb                 \
  %D%/automaton.lift.ipynb                      \
  %D%/automaton.minimize.ipynb                  \
  %D%/automaton.operators.ipynb                 \
  %D%/automaton.pair.ipynb                      \
  %D%/automaton.proper.ipynb                    \
  %D%/automaton.push_weights.ipynb              \
  %D%/automaton.reduce.ipynb                    \
  %D%/automaton.scc.ipynb                       \
  %D%/automaton.shortest.ipynb                  \
  %D%/automaton.shuffle.ipynb                   \
  %D%/automaton.standard.ipynb                  \
  %D%/automaton.synchronize.ipynb               \
  %D%/automaton.synchronizing_word.ipynb        \
  %D%/automaton.transpose.ipynb                 \
  %D%/automaton.trim.ipynb                      \
  %D%/context.cerny.ipynb                       \
  %D%/context.cotrie.ipynb                      \
  %D%/context.divkbaseb.ipynb                   \
  %D%/context.de_bruijn.ipynb                   \
  %D%/context.ladybird.ipynb                    \
  %D%/context.trie.ipynb                        \
  %D%/expression.automaton.ipynb                \
  %D%/expression.conjunction.ipynb              \
  %D%/expression.derivation.ipynb               \
  %D%/expression.derived_term.ipynb             \
  %D%/expression.expansion.ipynb                \
  %D%/expression.is_equivalent.ipynb            \
  %D%/expression.is_valid.ipynb                 \
  %D%/expression.multiply.ipynb                 \
  %D%/expression.operators.ipynb                \
  %D%/expression.shortest.ipynb                 \
  %D%/expression.standard.ipynb                 \
  %D%/expression.star_normal_form.ipynb         \
  %D%/expression.thompson.ipynb                 \
  %D%/expression.transpose.ipynb                \
  %D%/expression.transposition.ipynb            \
  %D%/expression.zpc.ipynb                      \
  %D%/index.ipynb                               \
  %D%/label.operators.ipynb                     \
  %D%/polynomial.cotrie.ipynb                   \
  %D%/polynomial.multiply.ipynb                 \
  %D%/polynomial.operators.ipynb                \
  %D%/polynomial.trie.ipynb                     \
  %D%/weight.operators.ipynb

if HAVE_NBCONVERT
nodist_notebooks_DATA = $(dist_notebooks_DATA:.ipynb=.html)
endif

## --------------- ##
## ipynb -> html.  ##
## --------------- ##


MATHJAX_BAD = <script src="https://c328740.ssl.cf1.rackcdn.com/mathjax/latest/MathJax.js?config=TeX-AMS_HTML"></script>
MATHJAX_OK = <script type='text/javascript' src='http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS_HTML'></script>
%.html: %.ipynb
	$(AM_V_GEN)$(mkdir_p) $(@D)
# nbconvert appends ".html" to the argument of --output.
	$(AM_V_at)$(NBCONVERT) $(if $(V:0=),,--quiet) --output="$*.tmp" "$<"
# The generated HTML files still point to ipynb files, which is ok for
# nbviewer, but not for us.  So s/ipynb/html/.
	$(AM_V_at)$(PERL) -pi					\
	   -e 's{(<a href=".*?\.)ipynb([#"])}{$$1html$$2}g;'	\
	   -e 's{\Q$(MATHJAX_BAD)\E}'"{$(MATHJAX_OK)}g;"	\
	   "$*.tmp.html"
	$(AM_V_at)mv -f "$*.tmp.html" "$@"

upload-doc: upload-notebooks
upload-notebooks: $(dist_notebooks_DATA) $(nodist_notebooks_DATA)
	ssh $(dload_host) mkdir -p $(dload_dir)/notebooks
	rsync $(RSYNCFLAGS) $^ $(dload_host):$(dload_dir)/notebooks
