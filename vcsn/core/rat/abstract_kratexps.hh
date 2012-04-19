#ifndef VCSN_CORE_RAT_FACTORY_HH
# define VCSN_CORE_RAT_FACTORY_HH

# include <string>
# include <list>

# include <vcsn/core/rat/node.fwd.hh>

namespace vcsn
{

  class abstract_kratexps
  {
  public:
    using exp_t = rat::exp;
    using value_t = exp_t*;

    virtual value_t zero() const = 0;
    virtual value_t unit() const = 0;
    virtual value_t atom(const std::string& w) const = 0;
    virtual value_t add(value_t l, value_t r) const = 0;
    virtual value_t mul(value_t l, value_t r) const = 0;
    virtual value_t star(value_t e) const = 0;
    virtual value_t weight(std::string* w, value_t e) const = 0;
    virtual value_t weight(value_t e, std::string* w) const = 0;

    virtual std::ostream& print(std::ostream& o, const value_t v) const = 0;
  };

} // namespace vcsn

#endif // !VCSN_CORE_RAT_FACTORY_HH
