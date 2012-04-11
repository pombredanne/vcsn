#ifndef VCSN_ALGOS_PRODUCT_HH
#define VCSN_ALGOS_PRODUCT_HH

#include <iostream>
#include <map>
#include <utility>
#include <deque>
#include "vcsn/core/mutable_automaton.hh"

namespace vcsn
{

  // This builds only the accessible part of the product.
  template <class A, class B>
  A
  product(const A& laut, const B& raut)
  {
    // FIXME: we should ensure that the alphabet and weightsets are
    // compatible.
    const auto& ws = laut.weightset();

    typedef mutable_automaton<typename A::genset_t,
                              typename A::weightset_t,
			      typename A::kind_t> automaton_t;
    automaton_t aut(laut.genset(), ws);
    typedef typename automaton_t::state_t state_t;

    typedef std::pair<typename A::state_t, typename B::state_t> pair_t;
    std::map<pair_t, typename automaton_t::state_t> pmap;

    pair_t ppre(laut.pre(), raut.pre());
    pair_t ppost(laut.post(), raut.post());
    pmap[ppre] = aut.pre();
    pmap[ppost] = aut.post();

    std::deque<pair_t> todo;
    todo.push_back(ppre);
    while (!todo.empty())
      {
	pair_t psrc = todo.front();
	todo.pop_front();
	state_t src = pmap[psrc];

	for (auto li : laut.all_out(psrc.first))
	  {
	    auto label = laut.label_of(li);
	    auto lweight = laut.weight_of(li);
	    auto ldst = laut.dst_of(li);

	    for (auto ri : raut.out(psrc.second, label))
	      {
		auto weight = ws.mul(lweight, raut.weight_of(ri));
		pair_t pdst(ldst, raut.dst_of(ri));

		auto iter = pmap.find(pdst);
		state_t dst;
		if (iter == pmap.end())
		  {
		    dst = aut.new_state();
		    pmap[pdst] = dst;
		    todo.push_back(pdst);
		  }
		else
		  {
		    dst = iter->second;
		  }
		aut.add_transition(src, dst, label, weight);
	      }
	  }
      }
    return aut;
  }
}

#endif // VCSN_ALGOS_PRODUCT_HH
