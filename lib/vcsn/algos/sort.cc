#include <lib/vcsn/algos/registry.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(is_out_sorted);
    bool
    is_out_sorted(const automaton& aut)
    {
      return detail::is_out_sorted_registry().call(aut);
    }

    REGISTER_DEFINE(sort);
    automaton
    sort(const automaton& aut)
    {
      return detail::sort_registry().call(aut);
    }
  }
}
