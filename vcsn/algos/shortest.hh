#pragma once

#include <algorithm>
#include <map>
#include <queue>

#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/labelset/word-polynomialset.hh>

namespace vcsn
{

  /*-----------------------.
  | enumerate(automaton).  |
  `-----------------------*/

  namespace detail
  {
    template <typename Aut>
    class enumerater
    {
    public:
      using automaton_t = Aut;
      using context_t = context_t_of<Aut>;
      static_assert(context_t::labelset_t::is_free(),
                    "enumerate: requires free labelset");

      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using wordset_context_t = word_context_t<context_t>;
      using polynomialset_t = polynomialset<wordset_context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using word_t = word_t_of<automaton_t>;

      using monomial_t = typename polynomialset_t::monomial_t;
      using queue_t = std::queue<std::pair<state_t, monomial_t>>;

      enumerater(const automaton_t& aut)
        : aut_(aut)
      {}

      /// The approximated behavior of the automaton.
      /// \param num   number of words looked for.
      /// \param len   maximum length of words looked for.
      polynomial_t operator()(size_t num, size_t len)
      {
        if (num == 0)
          num = len == 0 ? 1 : std::numeric_limits<unsigned>::max();
        if (len == 0)
          len = std::numeric_limits<unsigned>::max();

        // We match words that include the initial and final special
        // characters.
        if (len != std::numeric_limits<unsigned>::max())
          len += 2;

        queue_t queue;
        queue.emplace(aut_->pre(), ps_.monomial_one());

        for (size_t i = 0;
             !queue.empty() && i < len && output_.size() < num;
             ++i)
          propagate_(queue);

        // Return the past of post(), but remove the initial and final
        // special characters for the words.
        polynomial_t res;
        for (const auto& m: output_)
          {
            ps_.add_here(res, ls_.undelimit(label_of(m)), weight_of(m));
            if (--num == 0)
              break;
          }
        return res;
      }

    private:
      /// Process once all the states of \a q1.
      /// Save into q1 the new states to visit.
      void propagate_(queue_t& q1)
      {
        queue_t q2;
        while (!q1.empty())
          {
            auto sm = std::move(q1.front());
            state_t s = sm.first;
            const monomial_t& m = sm.second;
            q1.pop();
            for (const auto t: aut_->all_out(s))
              {
                // FIXME: monomial mul.
                monomial_t n(ls_.mul(label_of(m), aut_->label_of(t)),
                             ws_.mul(weight_of(m), aut_->weight_of(t)));
                if (aut_->dst_of(t) == aut_->post())
                  ps_.add_here(output_, n);
                q2.emplace(aut_->dst_of(t), n);
              }
          }
        q1.swap(q2);
      }

      /// The automaton whose behavior to approximate.
      const automaton_t& aut_;
      const weightset_t& ws_ = *aut_->weightset();
      const polynomialset_t ps_ = make_word_polynomialset(aut_->context());
      const labelset_t_of<polynomialset_t>& ls_ = *ps_.labelset();
      /// The approximated behavior: the first orders to post's past.
      polynomial_t output_;
    };
  }

  /// The approximated behavior of the automaton.
  /// \param num   number of words looked for.
  /// \param len   maximum length of words looked for.
  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::polynomial_t
  shortest(const Automaton& aut, unsigned num = 1, unsigned len = 0)
  {
    detail::enumerater<Automaton> enumerater(aut);
    return enumerater(num, len);
  }


  /// The approximated behavior of the automaton.
  /// \param len   maximum length of words looked for.
  template <typename Automaton>
  inline
  typename detail::enumerater<Automaton>::polynomial_t
  enumerate(const Automaton& aut, unsigned len)
  {
    return shortest(aut, 0, len);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Num, typename Len>
      polynomial
      shortest(const automaton& aut, unsigned num, unsigned len)
      {
        const auto& a = aut->as<Aut>();
        auto ps = vcsn::detail::make_word_polynomialset(a->context());
        return make_polynomial(ps, shortest(a, num, len));
      }
    }
  }
}