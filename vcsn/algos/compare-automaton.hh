#pragma once

#include <boost/range/algorithm/lexicographical_compare.hpp>

#include <vcsn/core/automaton.hh>

namespace vcsn
{

  /*---------------------------------.
  | compare(automaton, automaton).   |
  `---------------------------------*/

  /// Comparison between lhs and rhs.
  ///
  /// Comparing two automata for order is really dubious.  Even
  /// comparing for equality is really difficult, as is proved by
  /// are-isomorphic.  Yet, to be able to use automata as weights, we
  /// need to be able to order-compare two automata, and to hash an
  /// automaton.
  ///
  /// Both problem are ill-defined from a semantical point of view, in
  /// automata theory, but from an implementation point of view, we
  /// can decide for some order between automata.  For instance, by
  /// lexicograpical comparison of all their transitions.
  template <Automaton Lhs, Automaton Rhs>
  int
  compare(const Lhs& lhs, const Rhs& rhs)
  {
    return detail::lexicographical_cmp
      (lhs->all_transitions(),
       rhs->all_transitions(),
       [&lhs, &rhs](transition_t_of<Lhs> t1, transition_t_of<Rhs> t2)
       {
         // First, on src.
         if (auto res = int(lhs->src_of(t1)) - int(rhs->src_of(t2)))
           return res;
         // Second, on the label.
         else if (lhs->labelset()->less(lhs->label_of(t1), rhs->label_of(t2)))
           return -1;
         else if (lhs->labelset()->less(rhs->label_of(t2), lhs->label_of(t1)))
           return +1;
         // Third, on the weight.
         else if (lhs->weightset()->less(lhs->weight_of(t1), rhs->weight_of(t2)))
           return -1;
         else if (lhs->weightset()->less(rhs->weight_of(t2), lhs->weight_of(t1)))
           return +1;
         // Last, on dst.
         else
           return int(lhs->dst_of(t1)) - int(rhs->dst_of(t2));
       });
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (compare).
      template <Automaton Lhs, Automaton Rhs>
      int
      compare(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return ::vcsn::compare(l, r);
      }
    }
  }

  /// Whether lhs is equal to rhs.
  template <Automaton Lhs, Automaton Rhs>
  bool
  are_equal(const Lhs& lhs, const Rhs& rhs)
  {
    return compare(lhs, rhs) == 0;
  }

  /// Whether lhs precedes rhs.
  template <Automaton Lhs, Automaton Rhs>
  bool
  less_than(const Lhs& lhs, const Rhs& rhs)
  {
    return compare(lhs, rhs) < 0;
  }
} // namespace vcsn
