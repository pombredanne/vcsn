#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/algos/dotty.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/char_z_lal.hh>

using context_t = vcsn::ctx::char_z_lal;
using automaton_t = vcsn::mutable_automaton<context_t>;
using entry_t = automaton_t::entry_t;

bool
check_various(const context_t& ctx)
{
  bool res = true;
  automaton_t aut{ctx};

  auto s1 = aut.new_state();
  auto s2 = aut.new_state();
  auto s3 = aut.new_state();
  aut.set_initial(s1);
  aut.set_final(s2, 10);
  aut.set_transition(s1, s2, 'c', 42);
  aut.set_transition(s2, s3, 'a', 1);
  aut.set_transition(s2, s1, 'b', 1);
  int v = aut.add_weight(aut.set_transition(s1, s1, 'd', 2), 40);
  ASSERT_EQ(v, 42);
  aut.set_transition(s1, s3, 'd', 1);
  std::cout << vcsn::dotty(aut) << '\n';
  ASSERT_EQ(aut.num_states(), 3u);
  ASSERT_EQ(aut.num_transitions(), 5u);

  std::cout << "Leaving s1 by d" << std::endl;
  for (auto i: aut.out(s1, 'd'))
    {
      std::cout << i << " " << aut.dst_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cout << "Entering s1 by b" << std::endl;
  for (auto i: aut.in(s1, 'b'))
    {
      std::cout << i << " " << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }
  std::cout << "Between s1 and s1" << std::endl;
  for (auto i: aut.outin(s1, s1))
    {
      std::cout << i << " " << aut.src_of(i) << std::endl;
      assert(aut.has_transition(i));
    }

  aut.add_transition(s1, s1, 'd', -42);
  std::cout << vcsn::dotty(aut) << '\n';
  auto tj = aut.outin(s1, s1);
  assert(tj.begin() == tj.end());
  ASSERT_EQ(aut.num_states(), 3u);
  ASSERT_EQ(aut.num_transitions(), 4u);

  aut.del_state(s1);
  std::cout << vcsn::dotty(aut) << '\n';
  assert(!aut.has_state(s1));
  assert(aut.has_state(s2));
  assert(aut.has_state(s3));

  ASSERT_EQ(aut.num_states(), 2u);
  ASSERT_EQ(aut.num_transitions(), 1u);

  aut.set_transition(s2, s3, 'a', 0);

  std::cout << vcsn::dotty(aut) << '\n';
  ASSERT_EQ(aut.num_states(), 2u);
  ASSERT_EQ(aut.num_transitions(), 0u);
  return res;
}

bool
check_del_entry(const context_t& ctx)
{
  bool res = true;
  automaton_t aut{ctx};
  std::vector<automaton_t::state_t> s;

  size_t size = 3;
  for (size_t i = 0; i < size; ++i)
    s.push_back(aut.new_state());
  aut.set_initial(s[0]);
  aut.set_final(s[size-1]);

  aut.add_entry(s[0], s[1], aut.entryset().conv("{3}a+{4}b"));
  ASSERT_EQ(aut.weight_of(aut.get_transition(s[0], s[1], 'a')), 3);
  ASSERT_EQ(aut.weight_of(aut.get_transition(s[0], s[1], 'b')), 4);
  ASSERT_EQ(aut.has_transition(s[0], s[1], 'c'), false);

  aut.add_entry(s[1], s[2], aut.entryset().conv("{5}c+{6}d"));
  ASSERT_EQ(aut.weight_of(aut.get_transition(s[1], s[2], 'c')), 5);
  ASSERT_EQ(aut.weight_of(aut.get_transition(s[1], s[2], 'd')), 6);

  aut.del_entry(s[0], s[1]);
  ASSERT_EQ(aut.has_transition(s[0], s[1], 'a'), false);
  ASSERT_EQ(aut.has_transition(s[0], s[1], 'b'), false);
  ASSERT_EQ(aut.has_transition(s[0], s[1], 'c'), false);
  ASSERT_EQ(aut.weight_of(aut.get_transition(s[1], s[2], 'c')), 5);
  ASSERT_EQ(aut.weight_of(aut.get_transition(s[1], s[2], 'd')), 6);

  return res;
}


int main()
{
  size_t nerrs = 0;
  context_t ctx {{'a', 'b', 'c', 'd'}};
  nerrs += !check_various(ctx);
  nerrs += !check_del_entry(ctx);
  return !!nerrs;
}
