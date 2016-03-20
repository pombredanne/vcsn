#pragma once

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/labelset/nullableset.hh>
#include <vcsn/labelset/oneset.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/irange.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/random.hh>
#include <vcsn/misc/set.hh>

namespace vcsn
{

  /// Random label from oneset.
  template <typename RandomGenerator = std::default_random_engine>
  typename oneset::value_t
  random_label(const oneset& ls,
               RandomGenerator& = RandomGenerator())
  {
    return ls.one();
  }


  /// Random label from tupleset.
  template <typename... LabelSet,
            typename RandomGenerator = std::default_random_engine>
  typename tupleset<LabelSet...>::value_t
  random_label(const tupleset<LabelSet...>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    return random_label_(ls, gen, ls.indices);
  }


  /// Implementation detail for random label from tupleset.
  template <typename... LabelSet,
            size_t... I,
            typename RandomGenerator = std::default_random_engine>
  typename tupleset<LabelSet...>::value_t
  random_label_(const tupleset<LabelSet...>& ls,
                RandomGenerator& gen,
                detail::index_sequence<I...>)
  {
    // No need to check for the emptiness here: it will be checked in
    // each sub-labelset.
    return ls.tuple(random_label(ls.template set<I>(), gen)...);
  }


  /// Random label from wordset.
  template <typename GenSet,
            typename RandomGenerator = std::default_random_engine>
  typename wordset<GenSet>::value_t
  random_label(const wordset<GenSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    require(!ls.generators().empty(),
            "random_label: the alphabet needs at least 1 letter");
    auto dis = std::uniform_int_distribution<>(0, 5);
    auto res_label = ls.one();
    auto pick = make_random_selector(gen);
    for (auto _: detail::irange(dis(gen)))
      res_label = ls.mul(res_label, ls.value(pick(ls.generators())));
    return res_label;
  }


  /// Random label from general case such as letterset.
  template <typename LabelSet,
            typename RandomGenerator = std::default_random_engine>
  typename LabelSet::value_t
  random_label(const LabelSet& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    require(!ls.generators().empty(),
            "random_label: the alphabet needs at least 1 letter");
    // Pick a member of a container following a uniform distribution.
    auto pick = make_random_selector(gen);
    return ls.value(pick(ls.generators()));
  }


  /// Random label from nullableset.
  template <typename LabelSet,
            typename RandomGenerator = std::default_random_engine>
  typename nullableset<LabelSet>::value_t
  random_label(const nullableset<LabelSet>& ls,
               RandomGenerator& gen = RandomGenerator())
  {
    // FIXME: the proportion should be controllable.
    auto dis = std::bernoulli_distribution(0.5);
    if (dis(gen) || ls.generators().empty())
      return ls.one();
    else
      return ls.value(random_label(*ls.labelset(), gen));
  }


  /// Random label from expressionset: limited to a single label.
  template <typename Context,
            typename RandomGenerator = std::default_random_engine>
  typename expressionset<Context>::value_t
  random_label(const expressionset<Context>& rs,
               RandomGenerator& gen = RandomGenerator())
  {
    return rs.atom(random_label(*rs.labelset(), gen));
  }

  /*--------------------.
  | random_automaton.   |
  `--------------------*/

  /// Produce a random automaton.
  ///
  /// \param ctx
  ///    The context of the result.
  /// \param num_states
  ///    The number of states wanted in the automata
  ///    (>0).  All states will be connected, and there will be no dead
  ///    state.  However, some might not be coaccessible.
  /// \param density
  ///    The density of the automata.  This is the probability
  ///    (between 0.0 and 1.0), to add a transition between two
  ///    states.  All states have at least one outgoing transition, so
  ///    \a d is considered only when adding the remaining transition.
  ///    A density of 1 means all states will be connected to each
  ///    other.
  /// \param num_initial
  ///    The number of initial states wanted (0 <= num_initial <= num_states)
  /// \param num_final
  ///    The number of final states wanted (0 <= num_final <= num_states)
  /// \param loop_chance
  ///    The probability (between 0.0 and 1.0) for each state to have
  ///    a loop.
  template <typename Ctx>
  mutable_automaton<Ctx>
  random_automaton(const Ctx& ctx,
                   unsigned num_states, float density = 0.1,
                   unsigned num_initial = 1, unsigned num_final = 1,
                   float loop_chance = 0.0)
  {
    require(0 <= density && density <= 1,
            "random: density must be in [0,1]");
    require(0 <= loop_chance && loop_chance <= 1,
            "random: loop chance must be in [0,1]");

    using detail::irange;
    using automaton_t = mutable_automaton<Ctx>;
    using state_t = state_t_of<automaton_t>;
    auto res = make_shared_ptr<automaton_t>(ctx);

    auto& gen = make_random_engine();

    auto states = std::vector<state_t>{};
    states.reserve(num_states);
    // Indirect access to states[] to help random selection of successors.
    auto state_randomizer = std::vector<int>{};
    state_randomizer.reserve(num_states);

    // Using Sgi::hash_set instead of std::set for these sets is 3
    // times slower (tested on a 50000 states example).  These are
    // indexes in states[].
    using state_set = std::set<int>;
    state_set worklist;
    // Reachability from state[0] (_not_ from pre()).
    state_set unreachables;

    for (unsigned i: detail::irange(num_states))
      {
        states.emplace_back(res->new_state());
        state_randomizer.emplace_back(i);
        // State 0 is "reachable" from 0.
        if (i)
          unreachables.emplace(i);
        if (i < num_initial)
          res->set_initial(states[i]);
      }
    worklist.insert(0);

    // Select the final states.
    for (unsigned i: detail::irange(num_final))
      {
        auto dis = std::uniform_int_distribution<>(i, num_states - 1);
        int index = dis(gen);
        res->set_final(states[state_randomizer[index]]);
        // Swap it at the beginning of state_randomizer, so we cannot
        // pick it again.
        std::swap(state_randomizer[index], state_randomizer[i]);
      }

    // We want to connect each state to a number of successors between
    // 1 and n.  If the probability to connect to each successor is d,
    // the number of connected successors follows a binomial distribution.
    auto bin = std::binomial_distribution<>(num_states - 1, density);

    // Pick a member of a container following a uniform distribution.
    auto pick = make_random_selector(gen);

    while (!worklist.empty())
      {
        auto src = states[*worklist.begin()];
        worklist.erase(worklist.begin());

        // Choose a random number of successors (at least one), using
        // a binomial distribution.
        unsigned nsucc = 1 + bin(gen);

        // Connect to NSUCC randomly chosen successors.  We want at
        // least one unreachable successors among these if there are
        // some.
        bool saw_unreachable = false;
        auto possibilities = num_states;
        while (nsucc--)
          {
            // The index (in states[]) of the destination.
            unsigned dst = -1;
            // No connection to unreachable successors so far.  This is
            // our last chance, so force it now.
            if (nsucc == 0
                && !saw_unreachable
                && !unreachables.empty())
              {
                // Pick a random unreachable state.
                dst = pick.pop(unreachables);
                worklist.insert(dst);
              }
            else
              {
                // Pick the index of a random state.
                auto dis
                  = std::uniform_int_distribution<>(0, possibilities - 1);
                int index = dis(gen);
                possibilities--;

                dst = state_randomizer[index];

                // Permute it with state_randomizer[possibilities], so
                // we cannot pick it again.
                std::swap(state_randomizer[index],
                          state_randomizer[possibilities]);

                state_set::iterator j = unreachables.find(dst);
                if (j != unreachables.end())
                  {
                    worklist.insert(dst);
                    unreachables.erase(j);
                    saw_unreachable = true;
                  }
              }
            res->add_transition(src, states[dst],
                                random_label(*ctx.labelset(), gen));
          }
      }

    // Add loops.
    if (0 < loop_chance)
      {
        auto dis = std::bernoulli_distribution(loop_chance);
        for (auto s : res->states())
          if (dis(gen))
            res->add_transition(s, s, random_label(*ctx.labelset(), gen));
      }

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx, typename, typename, typename, typename, typename>
      automaton
      random_automaton(const context& ctx,
                       unsigned num_states, float density,
                       unsigned num_initial, unsigned num_final,
                       float loop_chance)
      {
        const auto& c = ctx->as<Ctx>();
        return vcsn::random_automaton(c, num_states, density, num_initial,
                                      num_final, loop_chance);
      }
    }
  }


  /*----------------------------------.
  | random_automaton_deterministic.   |
  `----------------------------------*/

  template <typename Ctx>
  mutable_automaton<Ctx>
  random_automaton_deterministic(const Ctx& ctx, unsigned num_states)
  {
    require(0 < num_states, "num_states must be > 0");

    using automaton_t = mutable_automaton<Ctx>;
    using state_t = state_t_of<automaton_t>;
    automaton_t res = make_shared_ptr<automaton_t>(ctx);

    auto& gen = make_random_engine();
    auto dis = std::uniform_int_distribution<int>(0, num_states - 1);

    auto states = std::vector<state_t>{};
    states.reserve(num_states);

    for (auto _: detail::irange(num_states))
      states.emplace_back(res->new_state());

    for (auto i: detail::irange(num_states))
      for (auto l : ctx.labelset()->generators())
        res->add_transition(states[i], states[dis(gen)], l,
                            ctx.weightset()->one());

    res->set_initial(states[dis(gen)]);
    res->set_final(states[dis(gen)]);

    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge
      template <typename Ctx, typename>
      automaton
      random_automaton_deterministic(const context& ctx, unsigned num_states)
      {
        const auto& c = ctx->as<Ctx>();
        return vcsn::random_automaton_deterministic(c, num_states);
      }
    }
  }
}
