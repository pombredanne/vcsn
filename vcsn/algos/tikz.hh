#ifndef VCSN_ALGOS_TIKZ_HH
# define VCSN_ALGOS_TIKZ_HH

# include <algorithm>
# include <iostream>
# include <unordered_map>
# include <unordered_set>
# include <vector>

// FIXME: factor dot and tikz.
# include <vcsn/algos/grail.hh> // outputter
# include <vcsn/algos/dot.hh> // format_entry

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*--------------------------.
  | tikz(automaton, stream).  |
  `--------------------------*/

  namespace detail
  {
    /// \brief Format automaton to TikZ format.
    ///
    /// \tparam Aut an automaton type, not a pointer type.
    template <typename Aut>
    class tikzer: public outputter<Aut>
    {
    public:
      using super_type = outputter<Aut>;
      using typename super_type::automaton_t;
      using typename super_type::state_t;
      using typename super_type::transition_t;
      using typename super_type::weight_t;

      using super_type::aut_;
      using super_type::format_entry_;
      using super_type::os_;
      using super_type::ws_;

      using super_type::super_type;

      /// Format a TikZ attribute.
      /// \param kind  the attribute name (e.g., "initial").
      /// \param w     the associated weight (e.g., initial weight).
      void format(const std::string& kind, const weight_t& w)
      {
        if (!ws_.is_zero(w))
          {
            os_ << ',' << kind;
            if (ws_.show_one() || !ws_.is_one(w))
              {
                os_ << ',' << kind << " text=$";
                ws_.print(w, os_) << '$';
              }
          }
      }

      void operator()()
      {
        os_ <<
          "\\documentclass{standalone}\n"
          "  \\usepackage{tikz}\n"
          "  \\usetikzlibrary{arrows, automata, positioning}\n"
          "  \\tikzstyle{automaton}=[shorten >=1pt, pos=.4,\n"
          "                         >=stealth', initial text=]\n"
          "  \\tikzstyle{accepting}=[accepting by arrow]\n"
          "\n"
          "\\begin{document}\n"
          "\\begin{tikzpicture}[automaton]\n"
          ;

        for (auto s : aut_->states())
          {
            os_ << "  \\node[state";
            format("initial", aut_->get_initial_weight(s));
            format("accepting", aut_->get_final_weight(s));
            os_ << "] (";
            aut_->print_state(s, os_);
            os_ << ')';
            if (2 < s)
              {
                os_ << " [right=of ";
                aut_->print_state(s - 1, os_);
                os_ << ']';
              }
            os_ << " {$";
            aut_->print_state(s, os_);
            os_ << "$};\n";
        }

        for (auto src : aut_->states())
          {
            std::set<state_t> ds;
            for (auto t: aut_->out(src))
              ds.insert(aut_->dst_of(t));
            for (auto dst: ds)
              {
                os_ << "  \\path[->] (";
                aut_->print_state(src, os_);
                os_ << ')'
                    << " edge"
                    << (src == dst ? "[loop above]" : "")
                    << " node[above]"
                    << " {$" << format_entry_(src, dst) << "$}"
                    << " (";
                aut_->print_state(dst, os_);
                os_ << ");\n";
              }
          }
        os_ <<
          "\\end{tikzpicture}\n"
          "\\end{document}";
      }
    };
  }

  /// \brief Format automaton to TikZ format.
  ///
  /// \tparam AutPtr an automaton type.
  template <typename AutPtr>
  std::ostream&
  tikz(const AutPtr& aut, std::ostream& out)
  {
    detail::tikzer<AutPtr> t{aut, out};
    t();
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream>
      std::ostream& tikz(const automaton& aut, std::ostream& out)
      {
        return tikz(aut->as<Aut>(), out);
      }

      REGISTER_DECLARE(tikz,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_TIKZ_HH
