#ifndef VCSN_WEIGHTSET_FWD_HH
# define VCSN_WEIGHTSET_FWD_HH

namespace vcsn
{
  namespace detail
  {
    // b.hh.
    class b_impl;

    // f2.hh.
    class f2_impl;

    // q.hh.
    class q_impl;

    // r.hh.
    class r_impl;

    // z.hh.
    class z_impl;

    // zmin.hh.
    class zmin_impl;
  }

  // polynomialset.hh.
  template <class Context>
  class polynomialset;

  // weightset.hh.
  template <typename WeightSet>
  struct variadic_mul_mixin;

  using b    = variadic_mul_mixin<detail::b_impl>;
  using f2   = variadic_mul_mixin<detail::f2_impl>;
  using q    = variadic_mul_mixin<detail::q_impl>;
  using r    = variadic_mul_mixin<detail::r_impl>;
  using z    = variadic_mul_mixin<detail::z_impl>;
  using zmin = variadic_mul_mixin<detail::zmin_impl>;

} // namespace vcsn

#define VCSN_WEIGHTS_BINARY(Lhs, Rhs, Res)      \
  /** The join of two weightsets. */            \
  inline                                        \
  Res join(const Lhs&, const Rhs&)              \
  {                                             \
    return {};                                  \
  }

#endif // !VCSN_WEIGHTSET_FWD_HH
