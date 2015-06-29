#include <sstream>

#include <lib/vcsn/rat/driver.hh>
#include <lib/vcsn/rat/parse.hh>
#include <lib/vcsn/rat/scan.hh>
#include <vcsn/dyn/algos.hh> // make_context
#include <vcsn/misc/stream.hh> // fail_reading

namespace vcsn
{
  namespace rat
  {

    driver::driver(const dyn::context& ctx, rat::identities ids)
      : scanner_(new yyFlexLexer)
      , ctx_(ctx)
      , ids_(ids)
    {}

    driver::~driver()
    {}

    dyn::context driver::context() const
    {
      return ctx_;
    }

    void driver::context(const std::string& ctx)
    {
      ctx_ = dyn::make_context(ctx);
    }

    rat::identities driver::identities() const
    {
      return ids_;
    }

    void driver::error(const location& l, const std::string& m)
    {
      std::ostringstream er;
      er  << l << ": " << m;
      if (!!getenv("YYDEBUG"))
        std::cerr << er.str() << std::endl;
      errors += (errors.empty() ? "" : "\n") + er.str();
    }

    void driver::invalid(const location& l, const std::string& s)
    {
      error(l, "invalid input: " + s);
    }

    /// The nesting limit for parser traces, as specified per
    /// $YYDEBUG.
    static int
    debug_level_()
    {
      int res = 0;
      if (const char* cp = getenv("YYDEBUG"))
        {
          res = 1;
          std::istringstream s{cp};
          s >> res;
        }
      return res;
    }

    auto
    driver::parse(std::istream& is, const location& l)
      -> dyn::expression
    {
      scanner_->scan_open_(is);
      location_ = l;
      // Parser.
      parser p(*this);
      // If the number of nested parser invocations is less than
      // $YYDEBUG, be verbose.  Recursive parsings are used for
      // weights; for instance "<<a>b>c" uses three (nested) parsers.
      static int debug_level = debug_level_();
      static int nesting = 0;
      p.set_debug_level(debug_level && nesting < debug_level);
      ++nesting;
      if (p.parse())
        result_ = nullptr;
      scanner_->scan_close_();
      --nesting;

      return std::move(result_);
    }

    dyn::label
    driver::make_label(const location& loc, const std::string& s)
    {
      try
        {
          std::istringstream is{s};
          auto res = dyn::read_label(ctx_, is);
          if (is.peek() != -1)
            vcsn::fail_reading(is, "unexpected trailing characters in: ", s);
          return res;
        }
      catch (const std::exception& e)
        {
          throw parser::syntax_error(loc, e.what());
        }
    }

    dyn::expression
    driver::make_atom(const location& loc, const std::string& s)
    {
      return dyn::to_expression(ctx_, ids_, make_label(loc, s));
    }

    dyn::weight
    driver::make_weight(const location& loc, const std::string& s)
    {
      try
        {
          std::istringstream is{s};
          auto res = dyn::read_weight(ctx_, is);
          if (is.peek() != -1)
            vcsn::fail_reading(is, "unexpected trailing characters in: ", s);
          return res;
        }
      catch (const std::exception& e)
        {
          throw parser::syntax_error(loc, e.what());
        }
    }
  }
}
