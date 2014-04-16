#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic ignored "-Wmissing-declarations"

// python/vcsn_cxx.cc: In constructor 'minimize::minimize(const boost::python::detail::keywords<nkeywords>&, const char*)':
// python/vcsn_cxx.cc:445:1531: warning: typedef 'assertion' locally defined but not used [-Wunused-local-typedefs]
// BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(minimize, minimize, 0, 1);
# pragma GCC diagnostic ignored "-Wunused-local-typedefs"

# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#include <boost/python.hpp>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh> // vname
#include <vcsn/misc/stream.hh>

struct automaton;
struct context;
struct label;
struct polynomial;
struct ratexp;
struct weight;

/*----------.
| context.  |
`----------*/

struct context
{
  context(const vcsn::dyn::context& ctx)
    : ctx_(ctx)
  {}

  context(const std::string& ctx)
    : context(vcsn::dyn::make_context(ctx))
  {}

  automaton de_bruijn(unsigned n) const;

  automaton divkbaseb(unsigned divisor, unsigned base) const;

  automaton double_ring(unsigned n, const boost::python::list& finals) const;

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(ctx_, os, format);
    return os.str();
  }

  automaton ladybird(unsigned n) const;

  automaton random(unsigned num_states, float density = 0.1,
                   unsigned num_initial = 1, unsigned num_final = 1) const;

  label word(const std::string& s) const;

  vcsn::dyn::context ctx_;
};

/*------------.
| automaton.  |
`------------*/

struct automaton
{
  automaton(const vcsn::dyn::automaton& a)
    : aut_(a)
  {}

  automaton(const ratexp& r);

  automaton(const std::string& s, const std::string& format = "default")
  {
    std::istringstream is(s);
    aut_ = vcsn::dyn::read_automaton(is, format);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  automaton accessible() const
  {
    return vcsn::dyn::accessible(aut_);
  }

  automaton chain(unsigned n) const
  {
    return vcsn::dyn::chain(aut_, n);
  }

  automaton coaccessible() const
  {
    return vcsn::dyn::coaccessible(aut_);
  }

  automaton complement() const
  {
    return vcsn::dyn::complement(aut_);
  }

  automaton complete() const
  {
    return vcsn::dyn::complete(aut_);
  }

  automaton concatenate(const automaton& rhs) const
  {
    return vcsn::dyn::concatenate(aut_, rhs.aut_);
  }

  ::context context() const
  {
    return vcsn::dyn::context_of(aut_);
  }

  automaton determinize(bool complete = false) const
  {
    return vcsn::dyn::determinize(aut_, complete);
  }

  automaton difference(const automaton& rhs) const
  {
    return vcsn::dyn::difference(aut_, rhs.aut_);
  }

  automaton eliminate_state(int s) const
  {
    return vcsn::dyn::eliminate_state(aut_, s);
  }

  polynomial enumerate(unsigned max) const;

  weight eval(const label& l) const;

  std::string format(const std::string& format = "dot") const
  {
    std::ostringstream os;
    vcsn::dyn::print(aut_, os, format);
    return os.str();
  }

  automaton infiltration(const automaton& rhs) const
  {
    return vcsn::dyn::infiltration(aut_, rhs.aut_);
  }

  bool is_accessible() const
  {
    return vcsn::dyn::is_accessible(aut_);
  }

  bool is_ambiguous() const
  {
    return vcsn::dyn::is_ambiguous(aut_);
  }

  bool is_coaccessible() const
  {
    return vcsn::dyn::is_coaccessible(aut_);
  }

  bool is_complete() const
  {
    return vcsn::dyn::is_complete(aut_);
  }

  bool is_deterministic() const
  {
    return vcsn::dyn::is_deterministic(aut_);
  }

  bool is_empty() const
  {
    return vcsn::dyn::is_empty(aut_);
  }

  bool is_eps_acyclic() const
  {
    return vcsn::dyn::is_eps_acyclic(aut_);
  }

  bool is_equivalent(const automaton& rhs) const
  {
    return vcsn::dyn::are_equivalent(aut_, rhs.aut_);
  }

  bool is_isomorphic(const automaton& rhs) const
  {
    return vcsn::dyn::are_isomorphic(aut_, rhs.aut_);
  }

  bool is_normalized() const
  {
    return vcsn::dyn::is_normalized(aut_);
  }

  bool is_out_sorted() const
  {
    return vcsn::dyn::is_out_sorted(aut_);
  }

  bool is_proper() const
  {
    return vcsn::dyn::is_proper(aut_);
  }

  bool is_standard() const
  {
    return vcsn::dyn::is_standard(aut_);
  }

  bool is_synchronized_by(const label& word) const;

  bool is_trim() const
  {
    return vcsn::dyn::is_trim(aut_);
  }

  bool is_useless() const
  {
    return vcsn::dyn::is_useless(aut_);
  }

  bool is_valid() const
  {
    return vcsn::dyn::is_valid(aut_);
  }

  automaton left_mult(const weight& w) const;

  automaton minimize(const std::string& algo = "weighted") const
  {
    return vcsn::dyn::minimize(aut_, algo);
  }

  automaton pair() const
  {
    return vcsn::dyn::pair(aut_);
  }

  automaton power(unsigned n) const
  {
    return vcsn::dyn::power(aut_, n);
  }

  static automaton product_real(const boost::python::tuple& auts)
  {
    std::vector<vcsn::dyn::automaton> as;
    for (int i = 0; i < boost::python::len(auts); ++i)
      as.emplace_back(boost::python::extract<automaton>(auts[i])().aut_);
    return vcsn::dyn::product(as);
  }

  automaton proper(bool prune = true) const
  {
    return vcsn::dyn::proper(aut_, prune);
  }

  automaton right_mult(const weight& w) const;

  polynomial shortest(unsigned max) const;

  automaton shuffle(const automaton& rhs) const
  {
    return vcsn::dyn::shuffle(aut_, rhs.aut_);
  }

  automaton sort() const
  {
    return vcsn::dyn::sort(aut_);
  }

  automaton standard() const
  {
    return vcsn::dyn::standard(aut_);
  }

  automaton star() const
  {
    return vcsn::dyn::star(aut_);
  }

  automaton sum(const automaton& rhs) const
  {
    return vcsn::dyn::sum(aut_, rhs.aut_);
  }

  label synchronizing_word() const;

  ratexp to_ratexp() const;

  automaton transpose()
  {
    return vcsn::dyn::transpose(aut_);
  }

  automaton trim() const
  {
    return vcsn::dyn::trim(aut_);
  }

  automaton union_a(const automaton& rhs) const
  {
    return vcsn::dyn::union_a(aut_, rhs.aut_);
  }

  automaton universal() const
  {
    return vcsn::dyn::universal(aut_);
  }

  vcsn::dyn::automaton aut_;
};

/*--------.
| label.  |
`--------*/

struct label
{
  label(const vcsn::dyn::label& val)
    : val_(val)
  {}

  label(const context& ctx, const std::string& s)
  {
    std::istringstream is(s);
    val_ = vcsn::dyn::read_label(is, ctx.ctx_);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(os, val_, format);
    return os.str();
  }

  vcsn::dyn::label val_;
};

/*-------------.
| polynomial.  |
`-------------*/

struct polynomial
{
  polynomial(const vcsn::dyn::polynomial& val)
    : val_(val)
  {}

  polynomial(const context& ctx, const std::string& s)
  {
    std::istringstream is(s);
    val_ = vcsn::dyn::read_polynomial(is, ctx.ctx_);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  vcsn::dyn::polynomial val_;
};

/*---------.
| ratexp.  |
`---------*/

struct ratexp
{
  ratexp(const vcsn::dyn::ratexp& r)
    : r_(r)
  {}

  ratexp(const context& ctx, const std::string& r)
  {
    std::istringstream is(r);
    auto rs = vcsn::dyn::make_ratexpset(ctx.ctx_);
    r_ = vcsn::dyn::read_ratexp(is, rs);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  ratexp chain(int min, int max) const
  {
    return vcsn::dyn::chain(r_, min, max);
  }

  ratexp concatenate(const ratexp& rhs) const
  {
    return vcsn::dyn::concatenate(r_, rhs.r_);
  }

  ratexp conjunction(const ratexp& rhs) const
  {
    return vcsn::dyn::conjunction(r_, rhs.r_);
  }

  weight constant_term() const;

  ::context context() const
  {
    return vcsn::dyn::context_of(r_);
  }

  polynomial derivation(const label& l, bool breaking = true) const
  {
    return vcsn::dyn::derivation(r_, l.val_, breaking);
  }

  ratexp copy(const ::context& ctx)
  {
    auto rs = vcsn::dyn::make_ratexpset(ctx.ctx_);
    return vcsn::dyn::copy(r_, rs);
  }

  automaton derived_term(bool breaking = false) const
  {
    return vcsn::dyn::derived_term(r_, breaking);
  }

  ratexp difference(const ratexp& rhs) const
  {
    return vcsn::dyn::difference(r_, rhs.r_);
  }

  ratexp expand() const
  {
    return vcsn::dyn::expand(r_);
  }

  polynomial first_order(bool breaking = false) const
  {
    return vcsn::dyn::first_order(r_, breaking);
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(r_, os, format);
    return os.str();
  }

  bool is_equivalent(const ratexp& rhs) const
  {
    return vcsn::dyn::are_equivalent(r_, rhs.r_);
  }

  bool is_valid() const
  {
    return vcsn::dyn::is_valid(r_);
  }

  ratexp left_mult(const weight& w) const;

  automaton linear(bool breaking = false) const
  {
    return vcsn::dyn::linear(r_, breaking);
  }

  ratexp right_mult(const weight& w) const;

  polynomial split() const
  {
    return vcsn::dyn::split(r_);
  }

  automaton standard() const
  {
    return vcsn::dyn::standard(r_);
  }

  unsigned star_height() const
  {
    return vcsn::dyn::star_height(r_);
  }

  ratexp star_normal_form() const
  {
    return vcsn::dyn::star_normal_form(r_);
  }

  ratexp sum(const ratexp& rhs) const
  {
    return vcsn::dyn::sum(r_, rhs.r_);
  }

  automaton thompson() const
  {
    return vcsn::dyn::thompson(r_);
  }

  ratexp transpose()
  {
    return vcsn::dyn::transpose(r_);
  }

  vcsn::dyn::ratexp r_;
};

/*---------.
| weight.  |
`---------*/

struct weight
{
  weight(const vcsn::dyn::weight& val)
    : val_(val)
  {}

  weight(const context& ctx, const std::string& s)
  {
    std::istringstream is(s);
    val_ = vcsn::dyn::read_weight(is, ctx.ctx_);
    if (is.peek() != -1)
      vcsn::fail_reading(is, "unexpected trailing characters");
  }

  std::string format(const std::string& format = "text") const
  {
    std::ostringstream os;
    vcsn::dyn::print(val_, os, format);
    return os.str();
  }

  vcsn::dyn::weight val_;
};

/*---------------------------.
| automaton implementation.  |
`---------------------------*/

automaton::automaton(const ratexp& r)
{
  *this = r.derived_term();
}

polynomial automaton::enumerate(unsigned max) const
{
  return vcsn::dyn::enumerate(aut_, max);
}

weight automaton::eval(const label& l) const
{
  return vcsn::dyn::eval(aut_, l.val_);
}

bool automaton::is_synchronized_by(const label& word) const
{
  return vcsn::dyn::is_synchronized_by(aut_, word.val_);
}

automaton automaton::left_mult(const weight& w) const
{
  return vcsn::dyn::left_mult(w.val_, aut_);
}

automaton automaton::right_mult(const weight& w) const
{
  return vcsn::dyn::right_mult(aut_, w.val_);
}

polynomial automaton::shortest(unsigned max) const
{
  return vcsn::dyn::shortest(aut_, max);
}

label automaton::synchronizing_word() const
{
  return vcsn::dyn::synchronizing_word(aut_);
}

ratexp automaton::to_ratexp() const
{
  return vcsn::dyn::aut_to_exp(aut_);
}

/*-------------------------.
| context implementation.  |
`-------------------------*/

automaton context::de_bruijn(unsigned n) const
{
  return vcsn::dyn::de_bruijn(ctx_, n);
}

automaton context::divkbaseb(unsigned divisor, unsigned base) const
{
  return vcsn::dyn::divkbaseb(ctx_, divisor, base);
}

automaton context::double_ring(unsigned n, const boost::python::list& finals) const
{
  std::vector<unsigned> f;
  for (int i = 0; i < boost::python::len(finals); ++i)
    f.emplace_back(boost::python::extract<unsigned>(finals[i]));
  return vcsn::dyn::double_ring(ctx_, n, f);
}

automaton context::ladybird(unsigned n) const
{
  return vcsn::dyn::ladybird(ctx_, n);
}

automaton context::random(unsigned num_states, float density,
                          unsigned num_initial, unsigned num_final) const
{
  return vcsn::dyn::random_automaton(ctx_,
                                     num_states, density,
                                     num_initial, num_final);

}

label context::word(const std::string& s) const
{
  return label(context(vcsn::dyn::make_word_context(ctx_)), s);
}


/*------------------------.
| ratexp implementation.  |
`------------------------*/

weight ratexp::constant_term() const
{
  return vcsn::dyn::constant_term(r_);
}

ratexp ratexp::left_mult(const weight& w) const
{
  return vcsn::dyn::left_mult(w.val_, r_);
}

ratexp ratexp::right_mult(const weight& w) const
{
  return vcsn::dyn::right_mult(r_, w.val_);
}


/*-----------.
| vcsn_cxx.  |
`-----------*/

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(derivation, derivation, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(derived_term, derived_term, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(determinize, determinize, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(first_order, first_order, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(linear, linear, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(minimize, minimize, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(proper, proper, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(random_overloads, random, 1, 4);

BOOST_PYTHON_MODULE(vcsn_cxx)
{
  namespace bp = boost::python;

  bp::class_<automaton>
    ("automaton",
     bp::init<const ratexp&>())
    .def(bp::init<const std::string&, bp::optional<const std::string&>>())

    .def("accessible", &automaton::accessible)
    .def("chain", &automaton::chain)
    .def("coaccessible", &automaton::coaccessible)
    .def("complement", &automaton::complement)
    .def("complete", &automaton::complete)
    .def("concatenate", &automaton::concatenate)
    .def("context", &automaton::context)
    .def("determinize", &automaton::determinize, determinize())
    .def("difference", &automaton::difference)
    .def("eliminate_state", &automaton::eliminate_state)
    .def("enumerate", &automaton::enumerate)
    .def("eval_", &automaton::eval)
    .def("format", &automaton::format)
    .def("infiltration", &automaton::infiltration)
    .def("is_accessible", &automaton::is_accessible)
    .def("is_ambiguous", &automaton::is_ambiguous)
    .def("is_coaccessible", &automaton::is_coaccessible)
    .def("is_complete", &automaton::is_complete)
    .def("is_deterministic", &automaton::is_deterministic)
    .def("is_empty", &automaton::is_empty)
    .def("is_eps_acyclic", &automaton::is_eps_acyclic)
    .def("is_equivalent", &automaton::is_equivalent)
    .def("is_isomorphic", &automaton::is_isomorphic)
    .def("is_normalized", &automaton::is_normalized)
    .def("is_proper", &automaton::is_proper)
    .def("is_out_sorted", &automaton::is_out_sorted)
    .def("is_standard", &automaton::is_standard)
    .def("is_synchronized_by_", &automaton::is_synchronized_by)
    .def("is_trim", &automaton::is_trim)
    .def("is_useless", &automaton::is_useless)
    .def("is_valid", &automaton::is_valid)
    .def("left_mult", &automaton::left_mult)
    .def("minimize", &automaton::minimize, minimize())
    .def("pair", &automaton::pair)
    .def("power", &automaton::power)
    .def("product_real", &automaton::product_real).staticmethod("product_real")
    .def("proper_real", &automaton::proper, proper())
    .def("ratexp", &automaton::to_ratexp, "Conversion to ratexp.")
    .def("right_mult", &automaton::right_mult)
    .def("shortest", &automaton::shortest)
    .def("shuffle", &automaton::shuffle)
    .def("sort", &automaton::sort)
    .def("standard", &automaton::standard)
    .def("star", &automaton::star)
    .def("sum", &automaton::sum)
    .def("synchronizing_word", &automaton::synchronizing_word)
    .def("transpose", &automaton::transpose)
    .def("trim", &automaton::trim)
    .def("union", &automaton::union_a)
    .def("universal", &automaton::universal)
    ;

  bp::class_<context>
    ("context",
     bp::init<const std::string&>())
    .def("de_bruijn", &context::de_bruijn)
    .def("divkbaseb", &context::divkbaseb)
    .def("double_ring", &context::double_ring)
    .def("format", &context::format)
    .def("ladybird", &context::ladybird)
    .def("random", &context::random, random_overloads())
    .def("word", &context::word)
   ;

  bp::class_<label>
    ("label",
     bp::init<const context&, const std::string&>())
    .def("format", &label::format)
   ;

  bp::class_<ratexp>
    ("ratexp",
     bp::init<const context&, const std::string&>())
    .def("chain", &ratexp::chain)
    .def("concatenate", &ratexp::concatenate)
    .def("conjunction", &ratexp::conjunction)
    .def("constant_term", &ratexp::constant_term)
    .def("context", &ratexp::context)
    .def("copy", &ratexp::copy)
    .def("derivation_", &ratexp::derivation, derivation())
    .def("derived_term", &ratexp::derived_term, derived_term())
    .def("difference", &ratexp::difference)
    .def("expand", &ratexp::expand)
    .def("first_order", &ratexp::first_order, first_order())
    .def("format", &ratexp::format)
    .def("is_equivalent", &ratexp::is_equivalent)
    .def("is_valid", &ratexp::is_valid)
    .def("left_mult", &ratexp::left_mult)
    .def("linear", &ratexp::linear, linear())
    .def("right_mult", &ratexp::right_mult)
    .def("split", &ratexp::split)
    .def("standard", &ratexp::standard)
    .def("star_height", &ratexp::star_height)
    .def("star_normal_form", &ratexp::star_normal_form)
    .def("sum", &ratexp::sum)
    .def("thompson", &ratexp::thompson)
    .def("transpose", &ratexp::transpose)
    ;

  bp::class_<polynomial>
    ("polynomial",
     bp::init<const context&, const std::string&>())
    .def("format", &polynomial::format)
   ;

  bp::class_<weight>
    ("weight",
     bp::init<const context&, const std::string&>())
    .def("format", &weight::format)
   ;

}
