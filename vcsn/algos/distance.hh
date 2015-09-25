#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <boost/range/algorithm/max_element.hpp>

#include <vcsn/algos/copy.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/misc/algorithm.hh> // detail::back
#include <vcsn/misc/deque.hh>
#include <vcsn/misc/pair.hh>
#include <vcsn/weightset/nmin.hh>

namespace vcsn
{
  /// Single source shortest distance.
  /// Find shortest (weighted) path from state \a s0
  /// to all states of automaton \a aut.
  template <typename Aut>
  std::vector<weight_t_of<Aut>>
  ss_shortest_distance(const Aut& aut, state_t_of<Aut> s0)
  {
    using weight_t = weight_t_of<Aut>;
    using state_t = state_t_of<Aut>;

    auto ws = *aut->weightset();
    auto d = std::vector<weight_t>(aut->all_states().back() + 1, ws.zero());
    d[s0] = ws.one();
    auto r = d;

    auto todo = std::deque<state_t>{s0};
    while (!todo.empty())
      {
        auto s = todo.front();
        todo.pop_front();
        auto r1 = r[s];
        r[s] = ws.zero();
        for (auto t : aut->all_out(s))
          {
            auto dst = aut->dst_of(t);
            auto w1 = ws.mul(r1, aut->weight_of(t));
            auto w = ws.add(d[dst], w1);
            if (!ws.equal(d[dst], w))
              {
                d[dst] = w;
                r[dst] = ws.add(r[dst], w1);
                if (!has(todo, dst))
                  todo.emplace_back(dst);
              }
          }
    }
    return d;
  }

  /// Wrapper struct to provide the state distance function.
  template <typename Aut, typename WeightSet>
  struct state_distancer
  {
    state_distancer(const Aut& aut)
      : aut_(aut)
    {}

    using automaton_t = Aut;
    using weight_t = weight_t_of<automaton_t>;
    using state_t = state_t_of<automaton_t>;
    using pair_t = std::pair<state_t, weight_t>;

    /// State distance
    /// Find weighted distance between state \a s0 and state \a s1
    /// using a dfs.
    /// Assumes the subgraph accessible from \a s0 does not have cycles with a
    /// weight of 0.
    weight_t
    operator() (state_t s0, state_t s1) const
    {
      auto& ws = *aut_->weightset();

      // States to visit.
      auto stack = std::vector<pair_t>{{s0, ws.one()}};

      // Result weight
      weight_t res = ws.zero();

      while (!stack.empty())
      {
        pair_t p = stack.back();
        stack.pop_back();
        state_t src = p.first;
        weight_t w = p.second;

        // dfs
        for (auto t : aut_->all_out(src))
        {
          state_t dst = aut_->dst_of(t);
          weight_t new_weight(ws.mul(w, aut_->weight_of(t)));
          if (dst == s1)
            // Found a path, add it
            res = ws.add(res, new_weight);
          else
          {
            pair_t np(dst, new_weight);
            stack.emplace_back(dst, new_weight);
          }
        }
      }
      return res;
    }

  private:
    const automaton_t& aut_;
  };

  /// Struct specialization for nmin.
  template <typename Aut>
  struct state_distancer<Aut, nmin>
  {
    state_distancer(const Aut& aut)
      : aut_(aut)
    {}

    using automaton_t = Aut;
    using weight_t = weight_t_of<automaton_t>;
    using state_t = state_t_of<automaton_t>;
    using pair_t = std::pair<state_t, weight_t>;


    /// State distance on nmin
    /// Find weighted distance between state \a s0 and state \a s1
    /// using a dfs.
    /// The distance becomes a shortest path with only positive weights.
    weight_t
    operator() (state_t s0, state_t s1) const
    {
      auto& ws = *aut_->weightset();
      // stack of the states to visit
      auto stack = std::vector<pair_t>{{s0, ws.one()}};

      // map of the minimum distance found to get to the node
      // Initially, +inf for everyone
      auto min_weight
        = std::vector<weight_t>(detail::back(aut_->all_states()) + 1, ws.zero());

      while (!stack.empty())
      {
        pair_t p = stack.back();
        stack.pop_back();
        state_t src = p.first;
        weight_t w = p.second;

        // The state was not already seen with a smaller weight
        if (w < min_weight[src])
        {
          min_weight[src] = w;
          if (src != s1)
            for (auto t : aut_->all_out(src))
            {
              state_t dst = aut_->dst_of(t);
              weight_t new_weight(ws.mul(w, aut_->weight_of(t)));
              // Otherwise, useless since we have already seen a shorter path
              if (new_weight < min_weight[dst] &&
                  new_weight < min_weight[s1])
                stack.emplace_back(dst, new_weight);
            }
        }
      }
      return min_weight[s1];
    }

  private:
    const automaton_t& aut_;
  };

  /// Find the shortest paths from some states to all the states.
  ///
  /// \param aut    the automaton to traverse.
  /// \param start  the states from which we reach the order states.
  /// \returns a map that, for each state, gives a pair: the shortest distance
  ///          to its parent, and the transition id that allows to reach it.
  template<typename Aut>
  std::unordered_map<state_t_of<Aut>,
                     std::pair<unsigned,
                               transition_t_of<Aut>>>
  paths_ibfs(const Aut& aut, const std::vector<state_t_of<Aut>>& start)
  {
    using context_t = context_t_of<Aut>;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;

    std::queue<state_t> todo;
    std::unordered_set<state_t> marked;
    std::unordered_map<state_t, std::pair<unsigned, transition_t>> parent;

    for (auto s : start)
      todo.push(s);

    while (!todo.empty())
      {
        state_t p = todo.front();
        todo.pop();
        marked.insert(p);
        for (auto t : aut->all_in(p))
          {
            auto s = aut->src_of(t);
            if (marked.find(s) == marked.end())
              {
                todo.push(s);
                auto cur_p = parent.find(p);
                unsigned cur_d
                  = cur_p == parent.end() ? 1
                  : cur_p->second.first + 1;
                parent[s] = {cur_d, t};
              }
          }
      }
    return parent;
  }

  /// A shortest path between two states.
  ///
  /// \param aut    the automaton
  /// \param start  the starting state
  /// \param end    the target state
  template<typename Aut>
  std::vector<transition_t_of<Aut>>
  path_bfs(const Aut& aut,
           state_t_of<Aut> start, state_t_of<Aut> end)
  {
    using context_t = context_t_of<Aut>;
    using automaton_t =  mutable_automaton<context_t>;
    using state_t = state_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;

    std::queue<state_t> todo;
    std::unordered_set<state_t> marked;
    std::unordered_map<state_t, std::pair<state_t, transition_t>> parent;

    todo.push(start);
    while (!todo.empty())
      {
        state_t p = todo.front();
        todo.pop();
        marked.insert(p);
        if (p == end)
          {
            std::vector<transition_t> rpath;
            state_t bt_curr = end;
            while (bt_curr != start)
              {
                transition_t t;
                std::tie(bt_curr, t) = parent.find(bt_curr)->second;
                rpath.push_back(t);
              }
            std::reverse(rpath.begin(), rpath.end());
            return rpath;
          }
        else
          for (auto t : aut->all_out(p))
            {
              auto s = aut->dst_of(t);
              if (marked.find(s) == marked.end())
                {
                  todo.push(s);
                  parent[s] = {p, t};
                }
            }
      }
    // FIXME: why don't we raise here?
    return std::vector<transition_t>();
  }

  template <typename Aut>
  std::vector<std::vector<weight_t_of<Aut>>>
  all_distances(const Aut& aut)
  {
    using automaton_t = Aut;
    using weight_t = weight_t_of<automaton_t>;

    auto ws = aut->weightset();
    auto n = aut->all_states().back() + 1;
    std::vector<std::vector<weight_t>> res(
      n, std::vector<weight_t>(n, ws->zero()));

    for (auto t : aut->all_transitions())
      res[aut->src_of(t)][aut->dst_of(t)] = aut->weight_of(t);
    for (auto k : aut->all_states())
      for (auto i : aut->all_states())
        for (auto j : aut->all_states())
          res[i][j] = ws->add(res[i][j], ws->mul(res[i][k], res[k][j]));
    return res;
  }
}
