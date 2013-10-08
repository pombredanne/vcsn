#ifndef VCSN_CTX_LAO_BR_HH
# define VCSN_CTX_LAO_BR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/labelset/letterset.hh>
# include <vcsn/labelset/oneset.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_b = context<letterset<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::b>;
    using lao_br = context<oneset, vcsn::ratexpset<lal_char_b>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

# ifndef MAYBE_EXTERN
#  define MAYBE_EXTERN extern
# endif

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lao_br);
}

#endif // !VCSN_CTX_LAO_BR_HH
