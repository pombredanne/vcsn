#ifndef VCSN_CORE_KIND_HH
# define VCSN_CORE_KIND_HH

# include <istream>

# include <vcsn/misc/stream.hh>
# include <vcsn/misc/symbol.hh>

namespace vcsn
{

  /// Define the kinds, and auxiliary tools.
  ///
  /// is_ABBREV<Kinded>: Whether Kinded has a specific kind_t
  ///
  /// if_ABBREV<Kinded, R = Kinded>:
  /// Provide "overloading" on parameter types, for instance to require
  /// letter_t when labels_are_letters, and word_t when
  /// labels_are_words.  See expressionset::atom and expressionset::atom_ for
  /// an example.
  ///
  /// It is very tempting to turn these guys into members of
  /// context, but then, instead of "(if_lal<Ctx, letter_t> v)",
  /// one must write "(typename Cxx::template if_lal<letter_t> v)".

# define DEFINE(Abbrev, Name)                                           \
  struct labels_are_ ## Name                                            \
  {                                                                     \
    static symbol sname()                                               \
    {                                                                   \
      static symbol res(#Abbrev);                                       \
      return res;                                                       \
    }                                                                   \
                                                                        \
    static void make(std::istream& is)                                  \
    {                                                                   \
      /*                                                                \
       * name: lal_char(abc), expressionset<law_char(xyz), b>.              \
       *       ^^^ ^^^^ ^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^                \
       *        |   |    |        weightset                             \
       *        |   |    +-- gens                                       \
       *        |   +-- letter_type                                     \
       *        +-- kind                                                \
       *                                                                \
       * name: lao, expressionset<law_char(xyz), b>                         \
       *       ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^^^                         \
       *       kind         weightset                                   \
       */                                                               \
      eat(is, sname());                                                 \
    }                                                                   \
                                                                        \
  };                                                                    \
                                                                        \
  template <typename Kinded>                                            \
  struct is_ ## Abbrev                                                  \
    : std::is_same<typename Kinded::kind_t, labels_are_ ## Name>::type  \
  {}

  DEFINE(lal, letters);
  DEFINE(lan, nullable);
  DEFINE(lao, one);
  DEFINE(lar, expressions);
  DEFINE(lat, tuples);
  DEFINE(law, words);

# undef DEFINE
}

#endif // !VCSN_CORE_KIND_HH
