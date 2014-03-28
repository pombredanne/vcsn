#ifndef VCSN_CTX_LAL_CHAR_ZRR_HH
# define VCSN_CTX_LAL_CHAR_ZRR_HH

# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_z   = context<lal_char, vcsn::z>;
    using lal_char_zr  = context<lal_char, vcsn::ratexpset<lal_char_z>>;
    using lal_char_zrr = context<lal_char, vcsn::ratexpset<lal_char_zr>>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(ctx::lal_char_zrr);
};

#endif // !VCSN_CTX_LAL_CHAR_ZRR_HH
