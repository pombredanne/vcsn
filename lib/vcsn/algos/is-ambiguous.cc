#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(ambiguous_word);
    label
    ambiguous_word(const automaton& aut)
    {
      return detail::ambiguous_word_registry().call(aut);
    }

    REGISTER_DEFINE(is_ambiguous);
    bool
    is_ambiguous(const automaton& aut)
    {
      return detail::is_ambiguous_registry().call(aut);
    }

    REGISTER_DEFINE(is_cycle_ambiguous);
    bool
    is_cycle_ambiguous(const automaton& aut)
    {
      return detail::is_cycle_ambiguous_registry().call(aut);
    }
  }
}
