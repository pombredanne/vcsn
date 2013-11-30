#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <vcsn/core/rat/less-than.hh>
#include <vcsn/core/rat/ratexp.hh>
#include <vcsn/core/rat/transpose.hh>
#include <vcsn/dyn/algos.hh> // dyn::read_ratexp_string
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/ratexpset.hh> // dyn::make_ratexpset
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/stream.hh>

namespace vcsn
{
  template <typename Context>
  ratexpset<Context>::ratexpset(const context_t& ctx)
    : ctx_(ctx)
  {}

  template <typename Context>
  std::string
  ratexpset<Context>::sname()
  {
    return "ratexpset<" + context_t::sname() + '>';
  }

  template <typename Context>
  std::string
  ratexpset<Context>::vname(bool full) const
  {
    return "ratexpset<" + context().vname(full) + '>';
  }

  template <typename Context>
  ratexpset<Context>
  ratexpset<Context>::make(std::istream& is)
  {
    // name is, for instance, "ratexpset<lal_char(abcd)_z>".
    eat(is, "ratexpset<");
    auto ctx = Context::make(is);
    eat(is, '>');
    return {ctx};
  }

#define DEFINE                                  \
  template <typename Context>                   \
  inline                                        \
  auto                                          \
  ratexpset<Context>

  DEFINE::context() const -> const context_t&
  {
    return ctx_;
  }

  DEFINE::labelset() const -> const labelset_ptr&
  {
    return ctx_.labelset();
  }

  DEFINE::weightset() const -> const weightset_ptr&
  {
    return ctx_.weightset();
  }

  DEFINE::format(const value_t v) const -> std::string
  {
    std::ostringstream s;
    print(s, v);
    return s.str();
  }


  /*--------------------------------------------------------.
  | Implementation of ratexpset_base pure virtual methods.  |
  `--------------------------------------------------------*/

  DEFINE::atom(const label_t& v) const
    -> value_t
  {
    if (labelset()->is_one(v))
      return one();
    return std::make_shared<atom_t>(weightset()->one(), v);
  }

  DEFINE::zero() const
    -> value_t
  {
    return zero(weightset()->one());
  }

  DEFINE::one() const
    -> value_t
  {
    return one(weightset()->one());
  }

  DEFINE::zero(const weight_t& w) const
    -> value_t
  {
    return std::make_shared<zero_t>(w);
  }

  DEFINE::one(const weight_t& w) const
    -> value_t
  {
    return std::make_shared<one_t>(w);
  }


  template <typename Context>
  template <rat::exp::type_t Type>
  inline
  auto
  ratexpset<Context>::gather(ratexps_t& res, value_t v) const
    -> void
  {
    if (v->type() == Type)
      {
        const auto& nary = *down_pointer_cast<const nary_t<Type>>(v);
        if (weightset()->is_one(nary.left_weight())
            && weightset()->is_one(nary.right_weight()))
          res.insert(std::end(res), std::begin(nary), std::end(nary));
        else
          res.push_back(v);
      }
    else
      res.push_back(v);
  }

  template <typename Context>
  template <rat::exp::type_t Type>
  inline
  auto
  ratexpset<Context>::gather(value_t l, value_t r) const
    -> ratexps_t
  {
    ratexps_t res;
    gather<Type>(res, l);
    gather<Type>(res, r);
    return res;
  }

  DEFINE::add(value_t l, value_t r) const
    -> value_t
  {
    // Trivial Identity
    // E+0 = 0+E = E
    value_t res = nullptr;
    if (l->type() == type_t::zero)
      res = r;
    else if (r->type() == type_t::zero)
      res = l;
    // END: Trivial Identity
    else
      res = std::make_shared<sum_t>(weightset()->one(),
                                    weightset()->one(),
                                    gather<type_t::sum>(l, r));
    return res;
  }


  DEFINE::mul(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity: T in TAF-Kit doc.
    // E.0 = 0.E = 0.
    if (l->type() == type_t::zero)
      res = l;
    else if (r->type() == type_t::zero)
      res = r;
    // T: E.1 = 1.E = E.  Do not apply it, rather apply U_K:
    // E.({k}1) ⇒ E{k}, ({k}1).E ⇒ {k}E
    else if (r->type() == type_t::one)
      res = weight(l, r->left_weight());
    else if (l->type() == type_t::one)
      res = weight(l->left_weight(), r);
    // END: Trivial Identity
    else
      res = std::make_shared<prod_t>(weightset()->one(),
                                     weightset()->one(),
                                     gather<type_t::prod>(l, r));
    return res;
  }

  DEFINE::intersection(value_t l, value_t r) const
    -> value_t
  {
    value_t res = nullptr;
    // Trivial Identity.
    // E&0 = 0&E = 0.
    if (l->type() == type_t::zero)
      res = l;
    else if (r->type() == type_t::zero)
      res = r;
    // 1&1 = 1.
    else if (l->type() == type_t::one && r->type() == type_t::one)
      res = one(weightset()->mul(l->left_weight(), r->left_weight()));
    // a&a = a.
    else if (l->type() == type_t::atom && r->type() == type_t::atom)
      {
	auto lhs = down_pointer_cast<const atom_t>(l)->value();
	auto rhs = down_pointer_cast<const atom_t>(r)->value();
	if (labelset()->equals(lhs, rhs))
	  res = weight(l, r->left_weight());
	else
	  res = zero();
      }
    // 1&a = 0, a&1 = 0.
    else if ((l->type() == type_t::one && r->type() == type_t::atom)
             || (l->type() == type_t::atom && r->type() == type_t::one))
      res = zero();
    // END: Trivial Identity
    else
      res = std::make_shared<intersection_t>(weightset()->one(),
                                             weightset()->one(),
                                             gather<type_t::intersection>(l, r));
    return res;
  }

  DEFINE::concat(value_t l, value_t r) const
    -> value_t
  {
    return concat_(l, r, typename is_law<Context>::type{});
  }

  // Concatenation when not LAW.
  DEFINE::concat_(value_t l, value_t r, std::false_type) const
    -> value_t
  {
    return mul(l, r);
  }

  // Concatenation when LAW.
  DEFINE::concat_(value_t l, value_t r, std::true_type) const
    -> value_t
  {
    // Implicit concatenation between l and r.
    auto rt = r->type();
    auto lt = l->type();
    if (rt == type_t::atom
        && weightset()->is_one(r->left_weight()))
      {
        if (lt == type_t::atom
            && weightset()->is_one(l->left_weight()))
          return atom(labelset()->concat
                      (down_pointer_cast<const atom_t>(l)->value(),
                       down_pointer_cast<const atom_t>(r)->value()));
        else if (lt == type_t::prod)
          {
            const auto& prodl = *down_pointer_cast<const prod_t>(l);
            if (weightset()->is_one(prodl.left_weight())
                && weightset()->is_one(prodl.right_weight()))
              {
                // Concat of "(ab).a" and "b" is "(ab).(ab)".
                ratexps_t ratexps { prodl.begin(), prodl.end() };
                ratexps.back() = concat(ratexps.back(), r);
                return std::make_shared<prod_t>(weightset()->one(),
                                                weightset()->one(),
                                                ratexps);
              }
          }
        }
    // The following cases do not when parsing an expression.
    else if (rt == type_t::prod)
      {
        const auto& prodr = *down_pointer_cast<const prod_t>(r);
        if (weightset()->is_one(prodr.left_weight())
            && weightset()->is_one(prodr.right_weight()))
          {
            if (lt == type_t::atom
                && weightset()->is_one(l->left_weight()))
              {
                // Concat of "a" and "b.(ab)", is "(ab).(ab)".
                ratexps_t ratexps { prodr.begin(), prodr.end() };
                ratexps.front() = concat(l, ratexps.front());
                return std::make_shared<prod_t>(weightset()->one(),
                                                weightset()->one(),
                                                ratexps);
              }
            else if (lt == type_t::prod)
              {
                const auto& prodl = *down_pointer_cast<const prod_t>(l);
                if (weightset()->is_one(prodl.left_weight())
                    && weightset()->is_one(prodl.right_weight()))
                  {
                    // Concat of "(ab).a" and "b.(ab)" is "(ab).(ab).(ab)".
                    ratexps_t ratexps { prodl.begin(), prodl.end() };
                    ratexps.back() = concat(ratexps.back(), *prodr.begin());
                    ratexps.insert(ratexps.end(),
                                    prodr.begin() + 1, prodr.end());
                    return std::make_shared<prod_t>(weightset()->one(),
                                                    weightset()->one(),
                                                    ratexps);
                  }
              }
          }
      }
    // Fall back to explicit concatenation.
    return mul(l, r);
  }

  DEFINE::star(value_t e) const
    -> value_t
  {
    if (e->type() == type_t::zero)
      // Trivial one
      // (0)* == 1
      return one();
    else
      return std::make_shared<star_t>(weightset()->one(),
                                      weightset()->one(),
                                      e);
  }


  /*----------.
  | weights.  |
  `----------*/

  DEFINE::weight(const weight_t& w, value_t e) const
    -> value_t
  {
    // Trivial one $T_K$: {k}0 => 0, {0}x => 0.
    if (e->type() == type_t::zero || weightset()->is_zero(w))
      return zero();
    else
      {
        auto res = std::const_pointer_cast<node_t>(e->clone());
        res->left_weight() = weightset()->mul(w, e->left_weight());
        return res;
      }
  }

  DEFINE::weight(value_t e, const weight_t& w) const
    -> value_t
  {
    // Trivial one $T_K$: 0{k} => 0, x{0} => 0.
    if (e->type() == type_t::zero || weightset()->is_zero(w))
      return zero();
    else if (e->is_inner())
      {
        auto inner = down_pointer_cast<const inner_t>(e);
        auto res = std::const_pointer_cast<inner_t>(inner->clone());
        res->right_weight() = weightset()->mul(inner->right_weight(), w);
        return res;
      }
    else
      {
        // Not the same as calling weight(w, e), as the product might
        // not be commutative.
        using wvalue_t = typename node_t::wvalue_t;
        wvalue_t res = std::const_pointer_cast<node_t>(e->clone());
        res->left_weight() = weightset()->mul(e->left_weight(), w);
        return res;
      }
  }

  /*----------------------------------.
  | ratexpset as a WeightSet itself.  |
  `----------------------------------*/

  DEFINE::is_zero(value_t v) const
    -> bool
  {
    return v->type() == type_t::zero;
  }

  DEFINE::is_one(value_t v) const
    -> bool
  {
    return (v->type() == type_t::one
            && weightset()->is_one(v->left_weight()));
  }

  DEFINE::less_than(value_t lhs, value_t rhs)
    -> bool
  {
    using less_than_t = rat::less_than<ratexpset>;
    less_than_t lt;
    return lt(lhs, rhs);
  }

  DEFINE::conv(self_type, value_t v) const
    -> value_t
  {
    return v;
  }

  DEFINE::conv(b, typename b::value_t v) const
    -> value_t
  {
    return v ? one() : zero();
  }

  DEFINE::conv(std::istream& is) const
    -> value_t
  {
    auto dynres = dyn::read_ratexp(is, dyn::make_ratexpset(*this));
    const auto& res = dynres->template as<ratexpset>();
    return res.ratexp();
  }

  DEFINE::print(std::ostream& o, const value_t v,
		const std::string& format) const
    -> std::ostream&
  {
    using printer_t = rat::printer<ratexpset>;
    printer_t print(o, *this);
    print.format(format);
    return print(v);
  }

  DEFINE::transpose(const value_t v) const
    -> value_t
  {
    detail::transposer<ratexpset> tr{*this};
    return tr(v);
  }

#undef DEFINE

} // namespace vcsn
