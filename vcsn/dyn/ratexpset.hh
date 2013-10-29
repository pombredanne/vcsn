#ifndef VCSN_DYN_RATEXPSET_HH
# define VCSN_DYN_RATEXPSET_HH

# include <string>
# include <memory>  // shared_ptr

# include <vcsn/dyn/fwd.hh> // dyn::ratexp.
# include <vcsn/core/rat/fwd.hh> // rat::exp_t.

namespace vcsn
{
namespace dyn
{
namespace detail
{

  /// Abstract wrapper around a (typeful) ratexpset.
  ///
  /// Use it when you want to avoid depending on the ratexpset
  /// parameters (e.g., from a parser).  To use it, actually create a
  /// derived class (ratexpset_wrapper) with the given
  /// parameters, but handle as a reference to an ratexpset_base.
  class ratexpset_base
  {
  public:
    using value_t = rat::exp_t;

    virtual value_t zero() const = 0;
    virtual value_t one() const = 0;
    /// Throws std::domain_error if w is not a valid label_t.
    virtual value_t atom(const std::string& w) const = 0;
    virtual value_t add(value_t l, value_t r) const = 0;
    /// Explicit product.
    virtual value_t mul(value_t l, value_t r) const = 0;
    /// Implicit product.  If both \a l and \a r are weightless words,
    /// produce a new word that concatenates them.  Otherwise, use \a mul.
    virtual value_t concat(value_t l, value_t r) const = 0;
    virtual value_t star(value_t e) const = 0;
    virtual value_t weight(const std::string& w, value_t e) const = 0;
    virtual value_t weight(value_t e, const std::string& w) const = 0;

    virtual dyn::ratexp make_ratexp(const value_t& v) const = 0;

    /// Parsing.
    virtual value_t conv(const std::string& s) const = 0;

    virtual std::ostream& print(std::ostream& o, const value_t v) const = 0;
    std::string format(const value_t v) const;
  };

  /// Wrapper around a ratexpset.
  template <typename RatExpSet>
  class ratexpset_wrapper : public ratexpset_base
  {
  public:
    using ratexpset_t = RatExpSet;
    using context_t = typename ratexpset_t::context_t;
    using super_type = ratexpset_base;
    using label_t = typename context_t::label_t;
    using weight_t = typename context_t::weight_t;
    using value_t = typename super_type::value_t;
    /// Concrete value type.
    using node_t = typename context_t::node_t;

    /// Constructor.
    /// \param rs    the wrapped ratexpset.
    ratexpset_wrapper(const ratexpset_t& rs);

    /// From weak to strong typing.
    std::shared_ptr<const node_t>
    down(const value_t& v) const;

    /// From string, to typed weight.
    weight_t down(const std::string& w) const;

    virtual dyn::ratexp make_ratexp(const value_t& v) const override;

    /*--------------------------------------.
    | Specializations from ratexpset_base.  |
    `--------------------------------------*/

    virtual value_t zero() const override;
    virtual value_t one() const override;

    virtual value_t atom(const std::string& w) const override;

    virtual value_t add(value_t l, value_t r) const override;

    virtual value_t mul(value_t l, value_t r) const override;

    /// When concatenating two atoms, possibly make a single one,
    /// or make the product.
    virtual value_t concat(value_t l, value_t r) const override;

    virtual value_t star(value_t v) const override;

    virtual value_t weight(const std::string& w, value_t v) const override;

    virtual value_t weight(value_t v, const std::string& w) const override;

    /// Parsing.
    virtual value_t conv(const std::string& s) const override;

    virtual std::ostream& print(std::ostream& o, value_t v) const override;

  private:
    ratexpset_t rs_;
  };
} // namespace detail

  /// Build a dyn::ratexpset from its static ratexpset.
  template <typename RatExpSet>
  ratexpset make_ratexpset(const RatExpSet& rs);

} // namespace dyn
} // namespace vcsn

# include <vcsn/dyn/ratexpset.hxx>

#endif // !VCSN_DYN_RATEXPSET_HH
