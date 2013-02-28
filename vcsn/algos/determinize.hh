#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <functional> // std::hash
# include <unordered_map>
# include <set>
# include <stack>
# include <string>
# include <type_traits>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

namespace std
{
  // http://stackoverflow.com/questions/2590677
  template <class T>
  inline void hash_combine(std::size_t& seed, const T& v)
  {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }

  template <typename T>
  struct hash<set<T>>
  {
    size_t operator()(const set<T>& ss) const
    {
      size_t res = 0;
      for (auto s: ss)
        hash_combine(res, s);
      return res;
    }
  };
}

namespace vcsn
{

  template <class Aut>
  inline
  Aut
  determinize(const Aut& a)
  {
    static_assert(Aut::context_t::is_lal,
                  "requires labels_are_letters");
    static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                  "requires Boolean labels");

    using automaton_t = Aut;
    using state_t = typename automaton_t::state_t;
    using state_set = std::set<state_t>;
    using stack = std::stack<state_set>;
    using map = std::unordered_map<state_set, state_t>;

    const auto& letters = *a.labelset();
    automaton_t res{a.context()};
    // The stack of (input) sets of states waiting to be processed.
    stack st;
    // set of input states -> output state.
    map m;
    // Create a new output state from SS. Insert in the output
    // automaton, in the map, and push in the stack.
    auto push_new_state =
      [&res,&m,&a,&st] (const state_set& ss) -> state_t
      {
        state_t r = res.new_state();
        m[ss] = r;

        for (auto e : ss)
          if (a.is_final(e))
            {
              res.set_final(r);
              break;
            }
        st.push(ss);
        return r;
      };

    // The input initial states.
    state_set next;
    for (auto t : a.initial_transitions())
      next.insert(a.dst_of(t));
    res.set_initial(push_new_state(next));

    while (!st.empty())
      {
        auto ss = st.top();
        st.pop();
        for (auto l: letters)
          {
            next.clear();
            for (auto s : ss)
              for (auto t : a.out(s, l))
                next.insert(a.dst_of(t));
            auto i = m.find(next);
            state_t n = (i == m.end()) ? push_new_state(next) : i->second;

            res.add_transition(m[ss], n, l);
          }
      }
    return res;
  }

  /*-------------------.
  | dyn::determinize.  |
  `-------------------*/

  namespace dyn
  {
    namespace details
    {

      template <typename Aut>
      dyn::automaton
      determinize(const dyn::automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return make_automaton<Aut>(a.context(), determinize(a));
      }

      using determinize_t =
        auto (const dyn::automaton& aut) -> dyn::automaton;
      bool determinize_register(const std::string& ctx,
                                const determinize_t& fn);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
