#pragma once

#include <map>
#include <set>
#include <vector>

#include <vcsn/core/automaton-decorator.hh>

namespace vcsn
{
  namespace detail
  {

    /// An automaton wrapper whose states form a partition of the
    /// state set of another automaton.
    ///
    /// \tparam Aut the type of the wrapped input automaton.
    template <typename Aut>
    class partition_automaton_impl
      : public automaton_decorator<typename Aut::element_type::automaton_nocv_t>
    {
    public:
      /// Input automaton type.
      using automaton_t = Aut;
      /// Generated automaton type.
      using automaton_nocv_t
        = typename automaton_t::element_type::automaton_nocv_t;
      using context_t = context_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using super_t = automaton_decorator<automaton_nocv_t>;

      /// The underlying state type.
      using state_t = state_t_of<automaton_t>;

      /// The state names: a set of the original automaton states.
      using state_name_t = std::set<state_t>;

    private:
      /// A map from each state to the origin state set it stands for.
      using origins_t = std::map<state_t, state_name_t>;
      origins_t origins_;

      /// The input automaton.
      const automaton_t input_;

    public:
      partition_automaton_impl(const automaton_t& input)
        : super_t(input->context())
        , input_(input)
      {}

      /// Static name.
      static symbol sname()
      {
        static symbol res("partition_automaton<"
                          + automaton_t::element_type::sname() + '>');
        return res;
      }

      std::ostream& print_set(std::ostream& o, const std::string& format) const
      {
        o << "partition_automaton<";
        input_->print_set(o, format);
        return o << '>';
      }

      bool state_has_name(state_t s) const
      {
        return s != super_t::pre() && s != super_t::post();
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       const std::string& format = "text",
                       bool delimit = false) const
      {
        const auto& set = origins_.at(s);
        const char* separator = "";
        if (delimit)
          o << '{';
        for (auto s : set)
          {
            o << separator;
            input_->print_state_name(s, o, format, true);
            separator = ", ";
          }
        if (delimit)
          o << '}';
        return o;
      }

      using super_t::new_state;

      /// Make a new state representing the given input state set,
      /// which is required to be new -- no error-checking on this.
      state_t
      new_state(const state_name_t& set)
      {
        state_t res = new_state();
        origins_[res] = set;
        return res;
      }

      state_t
      new_state(const std::vector<state_t>& v)
      {
        state_name_t set;
        for (auto s: v)
          set.emplace(s);
        return new_state(std::move(set));
      }
    }; // class
  } // namespace detail

  /// A partition automaton as a shared pointer.
  template <typename Aut>
  using partition_automaton
    = std::shared_ptr<detail::partition_automaton_impl<Aut>>;

} // namespace vcsn
