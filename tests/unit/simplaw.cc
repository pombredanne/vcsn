#include <vcsn/weights/z.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <iostream>

int main()
{
  typedef vcsn::set_alphabet<vcsn::char_letters> alpha_t;

  alpha_t alpha{'a', 'b', 'c', 'd'};
  vcsn::mutable_automaton<alpha_t, vcsn::z, vcsn::labels_are_words> aut(alpha);

  auto s1 = aut.add_state();
  auto s2 = aut.add_state();
  auto s3 = aut.add_state();

  aut.set_initial(s1);
  aut.set_final(s2, 10);
  aut.set_transition(s1, s2, "ca", 42);
  aut.set_transition(s2, s3, "a", 1);
  aut.set_transition(s2, s1, "ba", 1);
  aut.set_transition(s3, s2, "", 2);

  vcsn::dotty(aut, std::cout);
}
