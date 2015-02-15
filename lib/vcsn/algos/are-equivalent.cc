#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(are_equivalent);
    bool
    are_equivalent(const automaton& lhs, const automaton& rhs)
    {
      return detail::are_equivalent_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(are_equivalent_expression);
    bool
    are_equivalent(const expression& lhs, const expression& rhs)
    {
      return detail::are_equivalent_expression_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(difference);
    automaton
    difference(const automaton& lhs, const automaton& rhs)
    {
      return detail::difference_registry().call(lhs, rhs);
    }

    REGISTER_DEFINE(difference_expression);
    expression
    difference(const expression& lhs, const expression& rhs)
    {
      return detail::difference_expression_registry().call(lhs, rhs);
    }
  }
}
