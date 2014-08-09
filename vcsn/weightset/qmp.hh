#ifndef VCSN_WEIGHTSET_QMP_HH
# define VCSN_WEIGHTSET_QMP_HH

# include <string>
# include <ostream>

# include <cstddef> // https://gcc.gnu.org/gcc-4.9/porting_to.html
# include <gmpxx.h>

# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/hash.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/star_status.hh>
# include <vcsn/misc/stream.hh> // eat
# include <vcsn/weightset/fwd.hh>
# include <vcsn/weightset/b.hh>
# include <vcsn/weightset/z.hh>
# include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
  class qmp_impl
  {
  public:
    using self_type = qmp;

    static std::string sname()
    {
      return "qmp";
    }

    std::string vname(bool = true) const
    {
      return sname();
    }

    /// Build from the description in \a is.
    static qmp make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = mpq_class;

    static value_t zero()
    {
      // Not value_t{0, 1} to avoid the (const char* s, int base)
      // constructor.
      return value_t{mpz_class(0), 1};
    }

    static value_t one()
    {
      return value_t{1, 1};
    }

    static value_t add(const value_t l, const value_t r)
    {
      return l + r;
    }

    static value_t sub(const value_t l, const value_t r)
    {
      return l - r;
    }

    static value_t mul(const value_t l, const value_t r)
    {
      return l * r;
    }

    static value_t
    rdiv(const value_t l, const value_t r)
    {
      require(!is_zero(r), "div: division by zero");
      return l / r;
    }

    static value_t
    ldiv(const value_t l, const value_t r)
    {
      return rdiv(r, l);
    }

    value_t star(const value_t v) const
    {
      if (abs(v.get_num()) < v.get_den())
        // No need to reduce: numerator and denominators are primes.
        return {v.get_den(), v.get_den() - v.get_num()};
      else
        raise(sname(), ": star: invalid value: ", format(*this, v));
    }

    static bool is_special(const value_t) // C++11: cannot be constexpr.
    {
      return false;
    }

    static bool is_zero(const value_t v)
    {
      return v.get_num() == 0;
    }

    static bool is_one(const value_t v)
    {
      // All values are normalized.
      return v.get_num() == 1 && v.get_den() == 1;
    }

    static bool equals(const value_t l, const value_t r)
    {
      return l == r;
    }

    /// Whether \a  < \a r.
    static bool less_than(value_t l, value_t r)
    {
      return l < r;
    }

    static constexpr bool is_commutative() { return true; }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::ABSVAL; }

    static value_t
    abs(const value_t v)
    {
      return ::abs(v);
    }

    static value_t
    transpose(const value_t v)
    {
      return v;
    }

    static size_t hash(value_t v)
    {
      // FIXME: be serious...
      return hash_value(format(qmp_impl(), v));
    }

    static value_t
    conv(self_type, value_t v)
    {
      return v;
    }

    static value_t
    conv(z, z::value_t v)
    {
      return {v, 1};
    }

    static value_t
    conv(b, b::value_t v)
    {
      return {v, 1};
    }

    static value_t
    conv(std::istream& i)
    {
      value_t res;
      i >> res;
      return res;
    }

    static std::ostream&
    print(const value_t v, std::ostream& o,
          const std::string& format = "text")
    {
      if (format == "latex")
        {
          if (v.get_den() == 1)
            o << v.get_num();
          else
            o << "\\frac{" << v.get_num() << "}{" << v.get_den() << '}';
        }
      else
        o << v;
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, const std::string& format = "text") const
    {
      if (format == "latex")
        o << "\\mathbb{Q}_{mp}";
      else if (format == "text")
        o << vname();
      else
        raise("invalid format: ", format);
      return o;
    }
  };
  }

  VCSN_WEIGHTS_BINARY(qmp, qmp, qmp);

  VCSN_WEIGHTS_BINARY(z, qmp, qmp);
  VCSN_WEIGHTS_BINARY(qmp, z, qmp);

  VCSN_WEIGHTS_BINARY(b, qmp, qmp);
  VCSN_WEIGHTS_BINARY(qmp, b, qmp);
}

#endif // !VCSN_WEIGHTSET_QMP_HH