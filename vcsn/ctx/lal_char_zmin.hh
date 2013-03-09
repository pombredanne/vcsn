#ifndef VCSN_CTX_LAL_CHAR_ZMIN_HH
# define VCSN_CTX_LAL_CHAR_ZMIN_HH

# include <vcsn/ctx/ctx.hh>
# include <vcsn/ctx/lal_char.hh>
# include <vcsn/weights/zmin.hh>

namespace vcsn
{
  namespace ctx
  {
    using lal_char_zmin = context<lal_char, vcsn::zmin>;
  }
}

# include <vcsn/ctx/instantiate.hh>

namespace vcsn
{
  VCSN_CTX_INSTANTIATE(lal_char_zmin);
};

#endif // !VCSN_CTX_LAL_CHAR_ZMIN_HH
