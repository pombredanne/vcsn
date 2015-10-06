#pragma once

#include <memory>
#include <set>

#include <vcsn/core/rat/fwd.hh>

// This file is dangerous, it prevents automatic instantiation of
// these algos.  It exists only to solve mutual dependencies (e.g.:
// proper uses is-valid which uses proper).
//
// FIXME: Solve these issues in some other way.

namespace vcsn
{
  // vcsn/algos/delay.hh
  namespace detail
  {
    template <typename Aut>
    class delay_automaton_impl;
  }

  /// An automaton wrapper that presents the delay automaton.
  template <typename Aut>
  using delay_automaton
    = std::shared_ptr<detail::delay_automaton_impl<Aut>>;

  // vcsn/algos/edit-automaton.hh.
  class automaton_editor;

  // vcsn/algos/focus.hh
  namespace detail
  {
    template <std::size_t Tape, typename Aut>
    class focus_automaton_impl;
  }

  /// A focus automaton as a shared pointer.
  template <std::size_t Tape, typename Aut>
  using focus_automaton
    = std::shared_ptr<detail::focus_automaton_impl<Tape, Aut>>;

  template <unsigned Tape, typename Aut>
  focus_automaton<Tape, Aut> focus(Aut aut);

  // vcsn/algos/epsilon-remover.hh.
  template <typename Aut>
  bool in_situ_remover(Aut& aut, bool prune = true);

  // vcsn/algos/info.hh.
  namespace detail_info
  {
    template <typename Aut>
    size_t
    num_eps_transitions(const Aut&);
  }

  // vcsn/algos/is-ambiguous.hh.
  template <typename Aut>
  bool is_ambiguous(const Aut& aut);

  // vcsn/algos/is-proper.hh.
  template <typename Aut>
  bool is_proper(const Aut& aut);

  // vcsn/algos/is-value.hh.
  template <typename Aut>
  bool is_valid(const Aut& aut);

  template <typename ExpSet>
  bool is_valid(const ExpSet&, const typename ExpSet::value_t&);

  // vcsn/algos/synchronize.hh
  namespace detail
  {
    template <typename Aut>
    class synchronized_automaton_impl;
  }

  /// An automaton wrapper that presents the synchronized automaton.
  template <typename Aut>
  using synchronized_automaton
    = std::shared_ptr<detail::synchronized_automaton_impl<Aut>>;

  // vcsn/algos/to-expansion.hh.
  template <typename ExpSet>
  typename rat::expansionset<ExpSet>::value_t
  to_expansion(const ExpSet& rs, const typename ExpSet::value_t& e);

  // vcsn/algos/transpose.hh
  namespace detail
  {
    template <typename Aut>
    class transpose_automaton_impl;
  }

  /// An automaton wrapper that presents the transposed automaton.
  template <typename Aut>
  using transpose_automaton
    = std::shared_ptr<detail::transpose_automaton_impl<Aut>>;

  /// A set of letter ranges.
  using letter_class_t = std::set<std::pair<std::string, std::string>>;
}
