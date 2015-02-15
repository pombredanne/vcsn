#include <vcsn/dyn/registers.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{

  /*----------------.
  | is_eps_acyclic. |
  `----------------*/

  namespace dyn
  {
    REGISTER_DEFINE(is_eps_acyclic);

    bool is_eps_acyclic(const automaton& aut)
    {
      return detail::is_eps_acyclic_registry().call(aut);
    }
  }
}
