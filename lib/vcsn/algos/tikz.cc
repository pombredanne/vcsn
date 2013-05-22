#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/tikz.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    /*-------.
    | tikz.  |
    `-------*/

    REGISTER_DEFINE(tikz);

    std::ostream&
    tikz(const automaton& aut, std::ostream& out)
    {
      detail::tikz_registry().call(aut->vname(),
                                   aut, out);
      return out;
    }
  }
}
