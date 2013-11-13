#include <lib/vcsn/algos/registry.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE2(determinize);

    automaton
    determinize(const automaton& aut, bool complete)
    {
      return detail::determinize_registry().call(vname(aut, complete),
                                                 aut, complete);
    }
  }
}
