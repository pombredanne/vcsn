#ifndef VCSN_DYN_SIGNATURE_PRINTER_HH
# define VCSN_DYN_SIGNATURE_PRINTER_HH

# include <ostream>

# include <vcsn/dyn/type-ast.hh>

namespace vcsn
{
  namespace ast
  {
    class signature_printer : public context_visitor
    {
    public:
      signature_printer(std::ostream& os, bool full)
        : os_(os), full_(full)
      {}

# define DEFINE(Type)                   \
      virtual void visit(const Type& t)

      DEFINE(automaton);
      DEFINE(context);
      DEFINE(genset);
      DEFINE(letterset);
      DEFINE(nullableset);
      DEFINE(oneset);
      DEFINE(other);
      DEFINE(polynomialset);
      DEFINE(expressionset);
      DEFINE(tupleset);
      DEFINE(weightset);
      DEFINE(wordset);

# undef DEFINE

    private:
      std::ostream& os_;
      bool full_;
    };

    /// The signature, normalized.
    ///
    /// For instance 'lan_char(ab), b' ->
    /// 'lan<letterset<char_letters(ab)>, b'
    std::string normalize(const std::string& sig);

  }
}

#endif // !VCSN_DYN_SIGNATURE_PRINTER_HH
