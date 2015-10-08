#pragma once

#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/algos/bellman-ford.hh>

namespace vcsn
{

  /*---------------------.
  | has_negative_cycle.  |
  `---------------------*/

  template <typename Aut>
  bool has_negative_cycle(const Aut& aut)
  {
    return !detail::bellman_ford_impl(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge
      template <typename Aut>
      bool has_negative_cycle(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::has_negative_cycle(a);
      }
    }
  }
}