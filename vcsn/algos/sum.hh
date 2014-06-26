#ifndef VCSN_ALGOS_SUM_HH
# define VCSN_ALGOS_SUM_HH

# include <map>

# include <vcsn/algos/product.hh> // join_automata
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp
# include <vcsn/dyn/weight.hh>
# include <vcsn/misc/raise.hh> // require

namespace vcsn
{

  /*----------------------------.
  | sum(automaton, automaton).  |
  `----------------------------*/

  /// Merge transitions of \a b into those of \a res.
  ///
  /// \precondition The context of \a res must include that of \a b.
  /// \precondition res and b must be standard.
  template <typename A, typename B>
  A&
  sum_here(A& res, const B& b)
  {
    require(is_standard(res), __func__, ": lhs must be standard");
    require(is_standard(b), __func__, ": rhs must be standard");

    // State in B -> state in Res.
    std::map<state_t_of<B>, state_t_of<A>> m;
    state_t_of<A> initial = res->dst_of(res->initial_transitions().front());
    for (auto s: b->states())
      m.emplace(s, b->is_initial(s) ? initial : res->new_state());
    m.emplace(b->pre(), res->pre());
    m.emplace(b->post(), res->post());

    // Add b.
    for (auto t: b->all_transitions())
      // Do not add initial transitions, the unique initial state is
      // already declared as such, and its weight must remain 1.
      if (b->src_of(t) != b->pre())
        {
          if (b->dst_of(t) == b->post())
            res->add_transition(m[b->src_of(t)], m[b->dst_of(t)],
                               b->label_of(t), b->weight_of(t));
          else
            res->new_transition(m[b->src_of(t)], m[b->dst_of(t)],
                               b->label_of(t), b->weight_of(t));
        }
    return res;
  }


  template <typename A, typename B>
  inline
  auto
  sum(const A& lhs, const B& rhs)
    -> decltype(join_automata(lhs, rhs))
  {
    auto res = join_automata(lhs, rhs);
    // A standard automaton has a single initial state.
    res->set_initial(res->new_state());
    sum_here(res, lhs);
    sum_here(res, rhs);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Lhs, typename Rhs>
      automaton
      sum(const automaton& lhs, const automaton& rhs)
      {
        const auto& l = lhs->as<Lhs>();
        const auto& r = rhs->as<Rhs>();
        return make_automaton(::vcsn::sum(l, r));
      }

      REGISTER_DECLARE(sum,
                       (const automaton&, const automaton&) -> automaton);
    }
  }


  /*------------------------------.
  | sum(polynomial, polynomial).  |
  `------------------------------*/

  /// Sums of values.
  template <typename ValueSet>
  inline
  typename ValueSet::value_t
  sum(const ValueSet& vs,
      const typename ValueSet::value_t& lhs,
      const typename ValueSet::value_t& rhs)
  {
    return vs.add(lhs, rhs);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename PolynomialSetLhs, typename PolynomialSetRhs>
      polynomial
      sum_polynomial(const polynomial& lhs, const polynomial& rhs)
      {
        const auto& l = lhs->as<PolynomialSetLhs>();
        const auto& r = rhs->as<PolynomialSetRhs>();
        auto rs = join(l.polynomialset(), r.polynomialset());
        auto lr = rs.conv(l.polynomialset(), l.polynomial());
        auto rr = rs.conv(r.polynomialset(), r.polynomial());
        return make_polynomial(rs, sum(rs, lr, rr));
      }

      REGISTER_DECLARE(sum_polynomial,
                       (const polynomial&, const polynomial&) -> polynomial);
    }
  }


  /*----------------------.
  | sum(ratexp, ratexp).  |
  `----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSetLhs, typename RatExpSetRhs>
      ratexp
      sum_ratexp(const ratexp& lhs, const ratexp& rhs)
      {
        const auto& l = lhs->as<RatExpSetLhs>();
        const auto& r = rhs->as<RatExpSetRhs>();
        auto rs = join(l.ratexpset(), r.ratexpset());
        auto lr = rs.conv(l.ratexpset(), l.ratexp());
        auto rr = rs.conv(r.ratexpset(), r.ratexp());
        return make_ratexp(rs, sum(rs, lr, rr));
      }

      REGISTER_DECLARE(sum_ratexp,
                       (const ratexp&, const ratexp&) -> ratexp);
    }
  }


  /*----------------------.
  | sum(weight, weight).  |
  `----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename WeightSetLhs, typename WeightSetRhs>
      weight
      sum_weight(const weight& lhs, const weight& rhs)
      {
        const auto& l = lhs->as<WeightSetLhs>();
        const auto& r = rhs->as<WeightSetRhs>();
        auto rs = join(l.weightset(), r.weightset());
        auto lr = rs.conv(l.weightset(), l.weight());
        auto rr = rs.conv(r.weightset(), r.weight());
        return make_weight(rs, sum(rs, lr, rr));
      }

      REGISTER_DECLARE(sum_weight,
                       (const weight&, const weight&) -> weight);
    }
  }
}

#endif // !VCSN_ALGOS_SUM_HH
