#ifndef VCSN_CTX_LAW_CHAR_BR_HH
# define VCSN_CTX_LAW_CHAR_BR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/wordset.hh>
# include <vcsn/weights/b.hh>

namespace vcsn
{
  namespace ctx
  {
    using law_char_b = context<WordSet<vcsn::set_alphabet<vcsn::char_letters>>,
                               vcsn::b>;
    using law_char_br = context<WordSet<vcsn::set_alphabet<vcsn::char_letters>>,
                                vcsn::ratexpset<law_char_b>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(law_char_br);
};

#endif // !VCSN_CTX_LAW_CHAR_BR_HH
