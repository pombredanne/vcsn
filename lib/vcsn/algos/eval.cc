#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/eval.hh>
#include <vcsn/dyn/algos.hh>
#include <lib/vcsn/algos/registry.hh>

namespace vcsn
{
  namespace dyn
  {
    REGISTER_DEFINE(eval);
    weight
    eval(const automaton& aut, const std::string& s)
    {
      return detail::eval_registry().call(aut, s);
    }
  }
}
