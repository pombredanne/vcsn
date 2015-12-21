#pragma once

#include <vcsn/algos/eval.hh>
#include <vcsn/algos/distance.hh>
#include <vcsn/algos/to-spontaneous.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/weightset/fwd.hh> // nmin

namespace vcsn
{
  template <Automaton Aut>
  inline
  auto
  weight_series(const Aut& a)
    -> std::enable_if_t<!std::is_same<weightset_t_of<Aut>, nmin>::value,
                         weight_t_of<Aut>>
  {
    auto aut = proper(to_spontaneous(a));
    return eval(aut);
  }

  template <Automaton Aut>
  inline
  auto
  weight_series(const Aut& a)
    -> std::enable_if_t<std::is_same<weightset_t_of<Aut>, nmin>::value,
                         weight_t_of<Aut>>
  {
    state_distancer<Aut> d(a);
    return d(a->pre(), a->post());
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      weight
      weight_series(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        auto res = ::vcsn::weight_series(a);
        const auto& ctx = a->context();
        return make_weight(*ctx.weightset(), res);
      }
    }
  }
} // namespace vcsn
