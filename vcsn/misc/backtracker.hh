#ifndef VCSN_MISC_BACKTRACKER_HH
# define VCSN_MISC_BACKTRACKER_HH

#include <cassert>
//#include <iostream>
//#include <map>
#include <stack>
#include <utility> // FIXME: remove unless needed

namespace vcsn LIBVCSN_API
{

  int candidate_solution_no = 0;

  /// This is meant to be used according to the "Curiously recurring
  /// template pattern": see (at least as of Spring 2013)
  /// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
  /// and
  /// https://en.wikipedia.org/wiki/Template_metaprogramming#Static_polymorphism .
  ///
  /// In practice a backtracker subclass C should inherit from
  /// backtracker<Solution, Move, C>.  This accomplishes static rather
  /// than dynamic dispatching on performance-critical method calls.
  template <typename Solution, typename Move, typename UndoInfo,
            typename Derived>
  class backtracker
  {
  public:
    using solution_t = Solution;
    using move_t = Move;
    using undo_info_t = UndoInfo;

  protected:
    struct move_action
    {
      enum class move_tag { push, undo } tag;
      move_t move;
      undo_info_t undo_info; // Only meaningful in the case of undo.
    };
    using move_action_t = move_action;

    solution_t solution_;

  private:
    using move_actions_t = std::stack<move_action_t>;
    move_actions_t move_actions;
    size_t depth_;
    void clear()
    {
      depth_ = 0;
      move_actions = decltype(move_actions)(); // std::stack has no clear method.
    }

    solution_t search()
    {
      if (nonvirtual_final(NULL))
        return std::move(solution_);
      nonvirtual_develop(NULL);
      while (! move_actions.empty())
        {
          const move_action_t ma = std::move(move_actions.top());
          move_actions.pop();

          switch (ma.tag)
            {
            case move_action::move_tag::push:
              push_undo_move(ma.move);
              ++ depth_;
              nonvirtual_do_move(solution_, ma.move);
              if (nonvirtual_final(& ma.move))
                return std::move(solution_);
              nonvirtual_develop(& ma.move);
              break;
            case move_action::move_tag::undo:
              -- depth_;
              nonvirtual_undo_move(solution_, ma.move, ma.undo_info);
              break;
            default:
              assert(false);
            } // switch
        } // while
      throw std::out_of_range("no solution");
    }

  public:
    solution_t operator()(const solution_t& initial_state)
    {
      clear();
      nonvirtual_set_initial(initial_state);

      return search();
    }

  protected:
    size_t depth()
    {
      return depth_;
    }

    void push_move(const move_t& m)
    {
      move_actions.push({move_action::move_tag::push, m, undo_info_t()});
    }

    void push_undo_move(const move_t& m)
    {
      move_actions.push({move_action::move_tag::undo, m,
                         std::move(nonvirtual_undo_info(solution_, m))});
    }

    // Convenience methods.  Reminder to ourselves: don't call virtual methods!
    void do_move(const move_t& c)
    {
      nonvirtual_do_move(solution_, c);
    }
    void undo_move(const move_t& c, const undo_info_t& ui)
    {
      nonvirtual_undo_move(solution_, c, ui);
    }

  private:
    /// Non-virtual wrappers for "statically-polymorphic" mehtods.
    /// Keeping these distinct from the abstract virtual methods
    /// is useful to force the user to override *all* virtual methods.
    inline void nonvirtual_do_move(solution_t& s, const move_t& c)
    {
      static_cast<Derived*>(this)->do_move(s, c);
    }
    inline void nonvirtual_undo_move(solution_t& s, const move_t& c,
                                     const undo_info_t& ui)
    {
      static_cast<Derived*>(this)->undo_move(s, c, ui);
    }
    inline const undo_info_t nonvirtual_undo_info(const solution_t& s,
                                                  const move_t& m)
    {
      static_cast<Derived*>(this)->undo_info(s, m);
    }
    inline void nonvirtual_set_initial(const solution_t& initial)
    {
      static_cast<Derived*>(this)->set_initial(initial);
    }
    inline bool nonvirtual_final(const move_t* latest_move)
    {
      return static_cast<Derived*>(this)->final(latest_move);
    }
    inline void nonvirtual_develop(const move_t* latest_move)
    {
      static_cast<Derived*>(this)->develop(latest_move);
    }

    /// Abstract methods, to be called in a non-virtual way thru the
    /// wrappers above.
    virtual void do_move(solution_t& s, const move_t& c) = 0;
    virtual void undo_move(solution_t& s, const move_t& c,
                           const undo_info_t& ui) = 0;
    virtual const undo_info_t undo_info(const solution_t& s,
                                        const move_t& m) = 0;
    virtual void set_initial(const solution_t& initial) = 0;
    virtual bool final(const move_t* latest_move) = 0;
    virtual void develop(const move_t* latest_move) = 0;
  }; // class

}

#endif // !VCSN_MISC_BACKTRACKER_HH
