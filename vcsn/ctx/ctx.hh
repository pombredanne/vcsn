#ifndef VCSN_CTX_CTX_HH
# define VCSN_CTX_CTX_HH

# include <cassert>
# include <memory>
# include <string>
# include <type_traits>

# include <vcsn/core/rat/fwd.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/ctx/fwd.hh>

namespace vcsn
{

  namespace dyn
  {
    class context
    {
    public:
      virtual std::string vname() const = 0;
      virtual std::string genset_string() const = 0;
    };
  }

  namespace ctx
  {
    template <typename GenSet,
              typename WeightSet,
              typename Kind>
    class context: public dyn::context
    {
    public:
      using genset_t = GenSet;
      using weightset_t = WeightSet;
      using genset_ptr = std::shared_ptr<const genset_t>;
      using weightset_ptr = std::shared_ptr<const weightset_t>;
      using kind_t = Kind;
      enum
        {
          is_lae = std::is_same<kind_t, labels_are_empty>::value,
          is_lal = std::is_same<kind_t, labels_are_letters>::value,
          is_law = std::is_same<kind_t, labels_are_words>::value,
        };

      /// Type of transition labels, and type of RatExp atoms.
      using label_t = typename label_trait<kind_t, genset_t>::label_t;
      using word_t = typename genset_t::word_t;
      /// Type of weights.
      using weight_t = typename weightset_t::value_t;
      /// Type of RatExp kratexps objects.
      using node_t = rat::node<label_t, weight_t>;
      using kratexp_t = std::shared_ptr<const node_t>;
      using kratexpset_t = typename vcsn::kratexpset<context>;
      /// Type of RatExp visitor.
      using const_visitor = vcsn::rat::const_visitor<label_t, weight_t>;

      context(const context& that)
        : gs_{that.gs_}
        , ws_{that.ws_}
      {}

      context(const genset_ptr& gs, const weightset_ptr& ws)
        : gs_{gs}
        , ws_{ws}
      {}

      context(const genset_t& gs = {}, const weightset_t& ws = {})
        : context{std::make_shared<const genset_t>(gs),
                  std::make_shared<const weightset_t>(ws)}
      {}

      /// The name of this context, built from its parameters.
      /// E.g., "char_b_lal", "char_zmin_law".
      static std::string sname()
      {
        return (genset_t::sname()
                + "_" + weightset_t::sname()
                + "_" + kind_t::sname());
      }

      virtual std::string vname() const override final
      {
        return sname();
      }

      virtual std::string genset_string() const override final
      {
        std::string res;
        for (auto l: *gs_)
          res += l;
        return res;
      }

      const genset_ptr& genset() const
      {
        return gs_;
      }
      const weightset_ptr& weightset() const
      {
        return ws_;
      }

      kratexpset_t
      make_kratexpset() const
      {
        return *this;
      }

      std::string format(const kratexp_t& e) const
      {
        kratexpset_t ks{*this};
        return ks.format(e);
      }

      kratexp_t
      downcast(const rat::exp_t& e) const
      {
        kratexp_t res = std::dynamic_pointer_cast<const node_t>(e);
        assert(res);
        return res;
      }

    private:
      genset_ptr gs_;
      weightset_ptr ws_;
    };

  }

  // Provide "overloading" on parameter types, for instance to require
  // letter_t when labels_are_letters, and word_t when
  // labels_are_words.  See kratexpset::atom and kratexpset::atom_ for
  // an example.
  //
  // It is very tempting to turns these guys into members of
  // ctx::context, but then, instead of "(if_lal<Ctx, letter_t> v)",
  // one must write "(typename Cxx::template if_lal<letter_t> v)".
  template <typename Ctx, typename R>
  using if_lae = typename std::enable_if<Ctx::is_lae, R>::type;

  template <typename Ctx, typename R>
  using if_lal = typename std::enable_if<Ctx::is_lal, R>::type;

  template <typename Ctx, typename R>
  using if_law = typename std::enable_if<Ctx::is_law, R>::type;
};

#endif // !VCSN_CTX_CTX_HH
