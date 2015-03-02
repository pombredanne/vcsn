#pragma once

#include <iterator>

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/traits.hh> // context_t_of
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/letterset.hh>
#include <vcsn/labelset/nullableset.hh>
#include <vcsn/labelset/wordset.hh>
#include <vcsn/misc/algorithm.hh> // detail::back

namespace vcsn
{

  namespace detail
  {
    template <typename In_Aut, typename Out_Aut>
    class letterizer
    {
    public:
      using in_automaton_t = In_Aut;
      using in_ctx_t = context_t_of<in_automaton_t>;
      using in_state_t = state_t_of<in_automaton_t>;
      using in_labelset_t = labelset_t_of<in_automaton_t>;
      using in_label_t = typename in_labelset_t::value_t;
      using in_transition_t = transition_t_of<in_automaton_t>;

      using out_automaton_t = Out_Aut;
      using out_ctx_t = context_t_of<out_automaton_t>;
      using out_state_t = state_t_of<out_automaton_t>;
      using out_labelset_t = labelset_t_of<out_automaton_t>;
      using out_labelset_ptr = std::shared_ptr<out_labelset_t>;
      using out_label_t = typename out_labelset_t::value_t;
      using out_transition_t = transition_t_of<out_automaton_t>;

      // map in_state_t -> out_state_t
      using map_t = std::vector<out_state_t>;

      letterizer(const in_automaton_t& in_aut, out_labelset_ptr ls)
        : in_aut_(in_aut)
        , out_aut_(make_mutable_automaton(out_ctx_t{ls, in_aut->weightset()}))
        , state_map_(detail::back(in_aut->all_states()) + 1)
      {}

      out_automaton_t letterize()
      {
        auto in_ls = in_aut_->labelset();
        auto out_ws = out_aut_->weightset();
        // Copy the states, and setup the map
        state_map_[in_aut_->pre()] = out_aut_->pre();
        state_map_[in_aut_->post()] = out_aut_->post();
        for (auto st : in_aut_->states())
          state_map_[st] = out_aut_->new_state();

        for (auto st : in_aut_->all_states())
          for (auto tr : in_aut_->all_out(st))
            {
              auto letters = in_ls->letters_of(in_aut_->label_of(tr),
                                               out_labelset_t::one());
              auto it = letters.begin();
              if (it != letters.end())
              {
                auto src = state_map_[st];
                auto dst = std::next(it) != letters.end()
                         ? out_aut_->new_state()
                         : state_map_[in_aut_->dst_of(tr)];
                out_aut_->new_transition(src, dst,
                                        *it, in_aut_->weight_of(tr));
                src = dst;
                for (++it; it != letters.end(); ++it)
                {
                  dst = std::next(it) == letters.end()
                      ? state_map_[in_aut_->dst_of(tr)]
                      : out_aut_->new_state();
                  out_aut_->new_transition(src, dst, *it, out_ws->one());
                  src = dst;
                }
              }
              else
                out_aut_->new_transition(state_map_[st],
                                        state_map_[in_aut_->dst_of(tr)],
                                        out_labelset_t::one(),
                                        in_aut_->weight_of(tr));
            }

        return std::move(out_aut_);
      }

    protected:
      const in_automaton_t& in_aut_;
      out_automaton_t out_aut_;
      map_t state_map_;
    };

    /// From an automaton, the corresponding automaton with a non-word labelset.
    template <typename LabelSet>
    struct letterized_labelset
    {
      static constexpr bool should_run = false;

      using labelset_t = LabelSet;
      static std::shared_ptr<labelset_t>
      labelset(const labelset_t& ls)
      {
        return std::make_shared<labelset_t>(labelset_t{ls.genset()});
      }

    };

    template <typename GenSet>
    struct letterized_labelset<letterset<GenSet>>
    {
      static constexpr bool should_run = false;

      using labelset_t = nullableset<letterset<GenSet>>;

      static std::shared_ptr<labelset_t>
      labelset(const letterset<GenSet>& ls)
      {
        return std::make_shared<labelset_t>(labelset_t{ls.genset()});
      }
    };

    template <typename GenSet>
    struct letterized_labelset<wordset<GenSet>>
    {
      static constexpr bool should_run = true;

      using labelset_t = nullableset<letterset<GenSet>>;

      static std::shared_ptr<labelset_t>
      labelset(const wordset<GenSet>& ls)
      {
        return std::make_shared<labelset_t>(labelset_t{ls.genset()});
      }
    };

    template <typename... LabelSets>
    struct letterized_labelset<tupleset<LabelSets...>>
    {
      using indices_t = make_index_sequence<sizeof...(LabelSets)>;

      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

      template <size_t I>
      using letterized_labelset_t =
          letterized_labelset<typename std::tuple_element<I, std::tuple<LabelSets...>>::type>;
      template <std::size_t... I>
      static constexpr bool should_run_(seq<I...>)
      {
        return any_<letterized_labelset_t<I>::should_run...>();
      }
      static constexpr bool should_run = should_run_(indices_t{});

      using labelset_t =
          tupleset<typename letterized_labelset<LabelSets>::labelset_t...>;

      static std::shared_ptr<labelset_t>
      labelset(const tupleset<LabelSets...>& ls)
      {
        return labelset_(ls, indices_t{});
      }

      template <std::size_t... I>
      static std::shared_ptr<labelset_t>
      labelset_(const tupleset<LabelSets...>& ls,
                seq<I...>)
      {
        return std::make_shared<labelset_t>(*letterized_labelset_t<I>::labelset(std::get<I>(ls.sets()))...);
      }
    };

    template <typename Aut>
    using letterized_ls = letterized_labelset<labelset_t_of<Aut>>;

    template <typename Aut>
    vcsn::enable_if_t<letterized_ls<Aut>::should_run,
            mutable_automaton<context<typename letterized_ls<Aut>::labelset_t,
                                      weightset_t_of<Aut>>>>
    letterize(const Aut& aut)
    {
      using labelset_t = typename letterized_ls<Aut>::labelset_t;
      letterizer<Aut, mutable_automaton<context<labelset_t, weightset_t_of<Aut>>>>
                lt(aut, letterized_ls<Aut>::labelset(*(aut->labelset())));
      return lt.letterize();
    }

    template <typename Aut>
    vcsn::enable_if_t<!letterized_ls<Aut>::should_run,
                      const Aut&>
    letterize(const Aut& aut)
    {
      return aut;
    }

  }

  /*------------.
  | letterize.  |
  `------------*/

  /// Split the word transitions in the input automaton into letter ones.
  ///
  /// \param[in] aut        the automaton
  /// \returns              the letterized automaton
  template <typename Aut>
  auto
  letterize(const Aut& aut)
    -> decltype(detail::letterize(aut))
  {
    return detail::letterize(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton letterize(const automaton& aut)
      {
        return make_automaton(::vcsn::letterize(aut->as<Aut>()));
      }
    }
  }

} // namespace vcsn
