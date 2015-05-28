#pragma once

#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/heap/fibonacci_heap.hpp>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/dot.hh>
#include <vcsn/algos/fwd.hh>
#include <vcsn/algos/is-eps-acyclic.hh>
#include <vcsn/algos/is-proper.hh>
#include <vcsn/algos/is-valid.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/debug-level.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/vector.hh> // make_vector

#define STATS

namespace vcsn
{
  namespace detail
  {
    /// This class contains the core of the proper algorithm.
    ///
    /// This class is specialized for labels_are_letter automata since
    /// all these methods become trivial.
    template <typename Aut,
              bool has_one = labelset_t_of<Aut>::has_one()>
    class epsilon_remover
    {
      using automaton_t = Aut;
      using state_t = state_t_of<automaton_t>;
      using weightset_t = weightset_t_of<automaton_t>;
      using weight_t = typename weightset_t::value_t;
      using labelset_t = labelset_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      using aut_proper_t = fresh_automaton_t_of<automaton_t,
                            detail::proper_context<context_t_of<automaton_t>>>;

    public:
      /// Get ready to eliminate spontaneous transitions.
      /// \param aut    the automaton in which to remove them
      /// \param prune  whether to delete states that become inaccessible
      epsilon_remover(automaton_t& aut, bool prune = true)
        : debug_(debug_level())
        , aut_(aut)
        , ws_(*aut->weightset())
        , empty_word_(aut->labelset()->one())
        , prune_(prune)
      {}

      aut_proper_t operator()()
      {
        auto proper_ctx = make_proper_context(aut_->context());
        auto res = make_shared_ptr<aut_proper_t>(proper_ctx);

        if (!is_proper(aut_))
          in_situ_remover_();

        copy_into(aut_, res);
        return res;
      }

      /**
         @brief The core of the (backward) epsilon-removal.

         For each state s
         if s has an epsilon-loop with weight w
         if w is not starrable, return false
         else compute ws = star(w)
         endif
         remove the loop
         else
         ws = 1
         endif
         for each incoming epsilon transition e:p-->s with weight h
         for each outgoing transition s--a|k-->q
         add (and not set) transition p-- a | h.ws.k --> q
         endfor
         if s is final with weight h
         add final weight h.ws to p
         endif
         remove e
         endfor
         endfor
         return true

         If the method returns false, \a aut is corrupted.

         @param aut The automaton in which epsilon-transitions will be removed
         @param prune   Whether to remove states that become inaccessible.
         @return true if the proper succeeds, or false otherwise.
      */

    private:
      /// Data needed to compare the elimination order between states.
      ///
      /// Code profiling shows that we spend too much time iterating
      /// on outgoing and inconing transitions of states to compute
      /// the order of elimination.  This structure caches what needs
      /// to be compared, and provides the comparison operator.
      struct state_profile
      {
        /// From the heap's top, recover state to eliminate.
        state_t state;
        /// Number of incoming spontaneous transitions.
        size_t in_sp;
        /// Number of incoming non-spontaneous transitions.
        size_t in_nsp;
        /// Number of outgoing spontaneous transitions.
        size_t out_sp;
        /// Number of outgoing non-spontaneous transitions.
        size_t out_nsp;

        /// \brief Generate a state profile.
        ///
        /// \param s  state handle
        /// \param insp  number of incoming spontaneous transitions to \a s
        /// \param in    number of incoming transitions to \a a
        /// \param outsp number of outgoing spontaneous transitions from \a s
        /// \param out   number of outgoing transitions from \a s
        state_profile(state_t s,
                      size_t insp, size_t in,
                      size_t outsp, size_t out)
          : state(s)
          , in_sp(insp), in_nsp(in - insp)
          , out_sp(outsp), out_nsp(out - outsp)
        {}

        void update(size_t insp, size_t in,
                    size_t outsp, size_t out)
        {
          in_sp = insp;
          in_nsp = in - insp;
          out_sp = outsp;
          out_nsp = out - outsp;
        }

        /// Whether l < r for the max-heap.
        ///
        /// Compare priorities: return true if \a r should be
        /// treated before \a l.  Must be strict.
        bool operator<(const state_profile& r) const
        {
          // (i) First, work on those with fewer outgoing spontaneous
          // transitions.
          // (ii) Prefer fewer outgoing transitions.
          // (iii) Prefer fewer incoming spontaneous transitions.
          // (iv) Then, ensure total order using state handle.
          //
          // Note the inversions between lhs and rhs.
          return (std::tie  (r.out_sp, r.out_nsp, r.in_sp, state)
                  < std::tie(out_sp,   out_nsp,   in_sp,   r.state));
        }

        friend std::ostream& operator<<(std::ostream& o, const state_profile& p)
        {
          return o << p.state
                   << 'o' << p.out_sp
                   << 'O' << p.out_nsp
                   << 'i' << p.in_sp
                   << 'I' << p.in_nsp;
        }
      };

      /// Update the profile of \a s.
      void update_profile_(state_t s)
      {
        if (auto p = profile(s))
          p->update(aut_->in(s, empty_word_).size(),
                    aut_->in(s).size(),
                    aut_->out(s, empty_word_).size(),
                    aut_->all_out(s).size());
      }

      /// Build the profiles and the heap for states with incoming
      /// spontaneous transitions.
      void build_heap_()
      {
        for (auto s: aut_->states())
          // We don't care about states without incoming spontaneous
          // transitions.
          if (auto in_sp = aut_->in(s, empty_word_).size())
            {
              auto in = aut_->in(s).size();
              auto out_sp = aut_->out(s, empty_word_).size();
              auto out = aut_->all_out(s).size();
              auto h = todo_.emplace(state_profile
                                     {s, in_sp, in, out_sp, out});
              handles_.emplace(s, h);
          }
      }

      /// Show the heap, for debugging.
      void show_heap_() const
      {
        const char* sep = "";
        for (auto i = todo_.ordered_begin(), end = todo_.ordered_end();
             i != end; ++i)
          {
            std::cerr << sep << *i;
            sep = " > ";
          }
      }

      /// Update the heap for \a s.
      /// \pre  its profile is updated.
      void update_heap_(state_t s)
      {
        if (3 < debug_)
          {
            std::cerr << "update heap (" << s << " : ";
            show_heap_();
          }
        auto i = handles_.find(s);
        if (i != handles_.end())
          todo_.update(i->second);
        if (3 < debug_)
          {
            std::cerr << ") => ";
            show_heap_();
            std::cerr << std::endl;
          }
      }

#ifdef STATS
      unsigned added_ = 0;
      unsigned removed_ = 0;
#endif

      state_profile*
      profile(state_t s)
      {
        auto i = handles_.find(s);
        if (i == handles_.end())
          return nullptr;
        else
          return &*i->second;
      }

      /// For each state (s), for each incoming epsilon-transitions
      /// (t), if (t) is a loop, the star of its weight is computed,
      /// otherwise, (t) is stored into the closure list.  Then (t) is
      /// removed.
      ///
      /// Because it is sometimes useful to be able to invoke proper
      /// on a single state, we kept this function free from any
      /// relation with the profiles and the heap.
      ///
      /// For some reason, we get poorer performances if this function
      /// is moved higher, before the state_profile definition.
      void in_situ_remover_(state_t s)
      {
        // Iterate on a copy, as we remove these transitions in the
        // loop.
        auto transitions = make_vector(aut_->in(s, empty_word_));
        // The star of the weight of the loop on 's' (1 if no loop).
        weight_t star = ws_.one();
        using state_weight_t = std::pair<state_t, weight_t>;
        std::vector<state_weight_t> closure;
        for (auto t : transitions)
          {
            weight_t weight = aut_->weight_of(t);
            state_t src = aut_->src_of(t);
            if (src == s)  //loop
              star = ws_.star(weight);
            else
              closure.emplace_back(src, weight);
            // Delete incoming epsilon transitions.
            aut_->del_transition(t);
          }

        /*
          For each transition (t : s -- label|weight --> dst),
          for each former
          epsilon transition closure->first -- e|closure->second --> s
          a transition
          (closure->first -- label | closure->second*weight --> dst)
          is added to the automaton (add, not set !!)

          If (s) is final with weight (weight),
          for each former
          epsilon transition closure->first -- e|closure->second --> s
          pair-second * weight is added to the final weight
          of closure->first
        */
        for (auto t: aut_->all_out(s))
          {
            // "Blowing": For each transition (or terminal arrow)
            // outgoing from (s), the weight is multiplied by
            // (star).
            weight_t blow = ws_.mul(star, aut_->weight_of(t));
            aut_->set_weight(t, blow);

            label_t label = aut_->label_of(t);
            state_t dst = aut_->dst_of(t);
            for (auto pair: closure)
              {
                state_t src = pair.first;
                weight_t w = ws_.mul(pair.second, blow);
                aut_->add_transition(src, dst, label, w);
              }
          }
#ifdef STATS
        unsigned added = aut_->all_out(s).size() * closure.size();
        unsigned removed = transitions.size();
#endif
        if (prune_ && aut_->all_in(s).empty())
          {
#ifdef STATS
            removed += aut_->all_out(s).size();
#endif
            aut_->del_state(s);
          }
#ifdef STATS
        added_ += added;
        removed_ += removed;
        if (1 < debug_)
          std::cerr << " -" << removed << "+" << added
                    << " (-" << removed_ << "+" << added_ << ")";
#endif
      }

      /// Remove all the states with incoming spontaneous transitions.
      ///
      /// Set-up and maintain a heap to process states in an order
      /// that attempts to avoid useless introducing useless
      /// spontaneous transitions.
      ///
      /// Think for instance of the applying proper to
      /// thompson(a?{n}): it is much more efficient to work "from
      /// final to initial states", than the converse (which is what
      /// the "implementation order" actually does).  For n=2000, the
      /// "implementation order" takes 102s on my machine, while this
      /// order (and its implementation) takes 15.2s.
      void in_situ_remover_()
      {
        build_heap_();
        /* For each state (s), for each incoming epsilon-transitions
           (t), if (t) is a loop, the star of its weight is computed,
           otherwise, (t) is stored into the closure list.  Then (t)
           is removed.  */

        // The neighbors of s: their profiles need to be updated after
        // s was processed.
        std::unordered_set<state_t> neighbors;
        while (!todo_.empty())
          {
            if (2 < debug_)
              {
                std::cerr << "Before: ";
                show_heap_();
                std::cerr << std::endl;
              }
            auto p = todo_.top();
            todo_.pop();
            if (1 < debug_)
              std::cerr << "Remove: " << p;

            auto s = p.state;
            handles_.erase(s);
            neighbors.clear();
            for (auto t: aut_->in(s))
              {
                state_t n = aut_->src_of(t);
                if (n != s)
                  neighbors.emplace(n);
              }
            for (auto t: aut_->out(s))
              {
                state_t n = aut_->dst_of(t);
                if (n != s)
                  neighbors.emplace(n);
              }

            in_situ_remover_(s);

            // Update all neighbors and then the heap.
            for (auto n: neighbors)
              update_profile_(n);
            for (auto n: neighbors)
              update_heap_(n);
            if (1 < debug_)
              std::cerr << " #tr: "
                        << num_spontaneous_transitions_()
                        << "/" << aut_->num_transitions() << std::endl;
            if (2 < debug_)
              {
                std::cerr << "After:  ";
                show_heap_();
                std::cerr << std::endl;
              }
            if (4 < debug_)
              dot(aut_, std::cerr) << std::endl;
            if (2 < debug_)
              std::cerr << std::endl;
          }
#ifdef STATS
        if (0 < debug_)
          std::cerr << "Total transitions -" << removed_
                    << "+" << added_ << std::endl;
#endif
      }

      /// The number of spontaneous transitions in aut_.
      ///
      /// Duplicates info.hh, but since info.hh uses this file, we get
      /// an insane mutual dependency.  Break it.
      size_t num_spontaneous_transitions_() const
      {
        size_t res = 0;
        for (auto t : aut_->transitions())
          res += aut_->labelset()->is_one(aut_->label_of(t));
        return res;
      }

    private:
      /// Debug level.  The higher, the more details are reported.
      int debug_;
      /// The automaton we work on.
      automaton_t& aut_;
      /// Shorthand to the weightset.
      const weightset_t& ws_;
      /// Shorthand to the empty word.
      label_t empty_word_;

      /// Max-heap to decide the order of state-elimination.
      using heap_t = boost::heap::fibonacci_heap<state_profile>;
      heap_t todo_;
      /// Map: state -> heap-handle.
      std::unordered_map<state_t, typename heap_t::handle_type> handles_;

      /// Whether to prune states that become inaccessible.
      bool prune_;
    };


    /*----------------------------------------------.
    | Specialization when there is no 'one' label.  |
    `----------------------------------------------*/

    template <typename Aut>
    class epsilon_remover<Aut, false>
    {
      using automaton_t = Aut;
      using aut_proper_t = fresh_automaton_t_of<automaton_t>;
    public:
      epsilon_remover(automaton_t& aut, bool = true)
        : aut_(aut)
      {}

      aut_proper_t operator()()
      {
        return copy(aut_);
      }

    private:
      const automaton_t& aut_;
    };
  }
}
