#ifndef VCSN_CORE_AUTOMATON_DECORATOR_HH
# define VCSN_CORE_AUTOMATON_DECORATOR_HH

# include <vcsn/ctx/traits.hh>

namespace vcsn
{
  namespace detail
  {
    template <typename AutPtr>
    class automaton_decorator
    {
    public:
      /// The type of automaton to wrap.
      using automaton_ptr = AutPtr;
      using automaton_t = typename automaton_ptr::element_type;

    protected:
      /// The wrapped automaton, possibly const.
      // Must be defined early to please decltype.
      automaton_ptr aut_;

    public:
      /// Used to workaround issues with decltype, see the const_casts
      /// above, and http://stackoverflow.com/questions/17111406.
      using automaton_nocv_t = typename automaton_t::self_nocv_t;

      /// The type of the automata to produce from this kind o
      /// automata.  For instance, determinizing a
      /// transpose_automaton<const mutable_automaton<Ctx>> should
      /// yield a transpose_automaton<mutable_automaton<Ctx>>, without
      /// the "inner" const.
      using self_nocv_t = automaton_decorator<std::shared_ptr<automaton_nocv_t>>;
      using context_t = context_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using weight_t = weight_t_of<automaton_t>;

      using labelset_t = labelset_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using kind_t = typename automaton_t::kind_t;

      using labelset_ptr = typename automaton_t::labelset_ptr;
      using weightset_ptr = typename automaton_t::weightset_ptr;

    public:
      automaton_decorator(automaton_ptr aut)
        : aut_{aut}
      {}

      automaton_decorator(const automaton_decorator& aut)
        : aut_(aut.aut_)
      {}

      automaton_decorator(const context_t& ctx)
        : aut_(std::make_shared<automaton_t>(ctx))
      {}

      automaton_decorator(automaton_decorator&& aut)
      {
        std::swap(aut_, aut.aut_);
      }

      automaton_decorator& operator=(automaton_decorator&& that)
      {
        if (this != &that)
          aut_ = std::move(that.aut_);
        return *this;
      }

      automaton_ptr
      original_automaton()
      {
        return aut_;
      }

      /*------------.
      | constexpr.  |
      `------------*/

# define DEFINE(Name)                                                   \
      template <typename... Args>                                       \
      static constexpr                                                  \
      auto                                                              \
      Name(Args&&... args)                                              \
        -> decltype(automaton_t::Name(std::forward<Args>(args)...))     \
      {                                                                 \
        return automaton_t::Name(std::forward<Args>(args)...);          \
      }

      DEFINE(null_state);
      DEFINE(null_transition);
      DEFINE(post);
      DEFINE(pre);
      DEFINE(sname);

#undef DEFINE

      /*--------.
      | const.  |
      `--------*/

# define DEFINE(Name)                                           \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args) const                                \
        -> decltype(aut_->Name(std::forward<Args>(args)...))    \
      {                                                         \
      return aut_->Name(std::forward<Args>(args)...);           \
    }

      DEFINE(all_in);
      DEFINE(all_out);
      DEFINE(all_states);
      DEFINE(all_transitions);
      DEFINE(context);
      DEFINE(dst_of);
      DEFINE(final_transitions);
      DEFINE(get_final_weight);
      DEFINE(get_initial_weight);
      DEFINE(get_transition);
      DEFINE(has_transition);
      DEFINE(has_state);
      DEFINE(in);
      DEFINE(initial_transitions);
      DEFINE(is_final);
      DEFINE(is_initial);
      DEFINE(label_of);
      DEFINE(labelset);
      DEFINE(num_all_states);
      DEFINE(num_finals);
      DEFINE(num_initials);
      DEFINE(num_states);
      DEFINE(num_transitions);
      DEFINE(out);
      DEFINE(outin);
      DEFINE(src_of);
      DEFINE(states);
      DEFINE(transitions);
      DEFINE(vname);
      DEFINE(weight_of);
      DEFINE(weightset);

# undef DEFINE


      /*------------.
      | non const.  |
      `------------*/

# define DEFINE(Name)                                           \
      template <typename... Args>                               \
      auto                                                      \
      Name(Args&&... args)                                      \
        -> decltype(aut_->Name(std::forward<Args>(args)...))    \
      {                                                         \
      return aut_->Name(std::forward<Args>(args)...);           \
      }

      DEFINE(add_final);
      DEFINE(add_initial);
      DEFINE(add_transition);
      DEFINE(add_transition_copy);
      DEFINE(add_weight);
      DEFINE(del_state);
      DEFINE(del_transition);
      DEFINE(lmul_weight);
      DEFINE(new_state);
      DEFINE(new_transition);
      DEFINE(new_transition_copy);
      DEFINE(rmul_weight);
      DEFINE(set_final);
      DEFINE(set_initial);
      DEFINE(set_transition);
      DEFINE(set_weight);
      DEFINE(unset_final);
      DEFINE(unset_initial);

# undef DEFINE
    };
  }
}

#endif // !VCSN_CORE_AUTOMATON_DECORATOR_HH
