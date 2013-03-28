#ifndef VCSN_FACTORY_LADYBIRD_HH
# define VCSN_FACTORY_LADYBIRD_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/core/mutable_automaton.hh>

namespace vcsn
{
  template <class Context>
  mutable_automaton<Context>
  ladybird(unsigned n, const Context& ctx)
  {
    static_assert(Context::is_lal || Context::is_lan,
                  "requires labels_are_letters or nullable");
    using context_t = Context;
    mutable_automaton<context_t> aut{ctx};

    auto p = aut.new_state();
    aut.set_initial(p);
    aut.set_final(p);
    auto x = p;
    for (unsigned i = 1; i < n; ++i)
      {
        auto y = aut.new_state();
        aut.add_transition(x, y, 'a');
        aut.add_transition(y, y, 'b');
        aut.add_transition(y, y, 'c');
        aut.add_transition(y, p, 'c');
        x = y;
      }
    aut.add_transition(x, p, 'a');
    return aut;
  }

  /*----------------.
  | dyn::ladybird.  |
  `----------------*/

  namespace dyn
  {
    namespace details
    {
      template <typename Ctx>
      automaton
      ladybird(const dyn::context& ctx, unsigned n)
      {
        const auto& c = dynamic_cast<const Ctx&>(*ctx);
        return make_automaton(c, ladybird<Ctx>(n, c));
      }

      using ladybird_t =
        auto (const dyn::context& ctx, unsigned n) -> automaton;

      bool ladybird_register(const std::string& ctx, ladybird_t fn);
    }
  }
}

#endif // !VCSN_FACTORY_LADYBIRD_HH
