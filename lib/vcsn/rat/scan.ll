%option nounput noyywrap
%option debug
%option prefix="rat" outfile="lex.yy.c"
%option stack noyy_top_state

%top{
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
}

%{
#include <string>
#include <cassert>
#include <stack>
#include <iostream>
#include <lib/vcsn/rat/parse.hh>
#include <vcsn/algos/dyn.hh>

#define LINE(Line)                              \
  do{                                           \
    yylloc->end.column = 1;                     \
    yylloc->lines(Line);                        \
 } while (false)

#define YY_USER_ACTION                          \
  yylloc->columns(yyleng);

#define TOK(Token)                              \
  vcsn::rat::parser::token::Token
%}
%x SC_CONTEXT SC_WEIGHT

char      ([a-zA-Z0-9_]|\\[{}()+.*:\"])

%%
%{
  // Count the number of opened braces in SC_WEIGHT, and parens in SC_CONTEXT.
  unsigned int nesting = 0;
  // Build a context string.  "static" only to save build/dtor.
  static std::string context;
  // Grow a string before returning it.
  std::string* sval = 0;

  yylloc->step();
%}

<INITIAL>{ /* Vcsn Syntax */

  "("     yylval->ival = 0; return TOK(LPAREN);
  ")"     yylval->ival = 0; return TOK(RPAREN);

  "["     yylval->ival = 1; return TOK(LPAREN);
  "]"     yylval->ival = 1; return TOK(RPAREN);
  "+"     return TOK(SUM);
  "."     return TOK(DOT);
  "*"     return TOK(STAR);
  "\\e"   return TOK(ONE);
  "\\z"   return TOK(ZERO);

  "(?@"   context.clear(); yy_push_state(SC_CONTEXT);
  "(?#"[^)]*")"  continue;

  "{"     sval = new std::string(); yy_push_state(SC_WEIGHT);
  {char}  yylval->cval = *yytext; return TOK(LETTER);
  "\n"    continue;
  \\.|.   driver_.invalid(*yylloc, yytext);

}

<SC_WEIGHT>{ /* Weight with Vcsn Syntax*/
  "{"                           {
    ++nesting;
    *sval += yytext;
  } // push brace
  "}"                           {
    if (nesting)
      {
        --nesting;
        *sval += yytext;
      }
    else
      {
        yy_pop_state();
        yylval->sval = sval;
        return TOK(WEIGHT);
      }
  }
  [^{}]+       *sval += yytext;
}

<SC_CONTEXT>{ /* Context embedded in a $(?@...) directive.  */
  "("   {
    ++nesting;
    context += yytext;
  }
  ")"   {
    if (nesting)
      {
        --nesting;
        context += yytext;
      }
    else
      {
        yy_pop_state();
        driver_.context(dyn::make_context(context));
        context.clear();
      }
  }
  [^()]+   context += yytext;
}

%%

namespace vcsn
{
  namespace rat
  {
    // Beware of the dummy Flex interface.  One would like to use:
    //
    // yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
    //
    // and
    //
    // yypush_buffer_state(yy_scan_bytes(e.c_str(), e.size()));
    //
    // but the latter (yy_scan_bytes) calls yy_switch_to_buffer, so in
    // effect calling yypush_buffer_state saves the new state instead
    // of the old one.
    //
    // So do it in two steps, quite different from what is suggested
    // in the documentation: save the old context, switch to the new
    // one.

    void
    driver::scan_open_(FILE *f)
    {
      yy_flex_debug = !!getenv("YYSCAN");
      yypush_buffer_state(YY_CURRENT_BUFFER);
      yy_switch_to_buffer(yy_create_buffer(f, YY_BUF_SIZE));
    }

    void
    driver::scan_open_(const std::string& e)
    {
      yy_flex_debug = !!getenv("YYSCAN");
      yyin = 0;
      yypush_buffer_state(YY_CURRENT_BUFFER);
      yy_scan_bytes(e.c_str(), e.size());
    }

    void
    driver::scan_close_()
    {
      yypop_buffer_state();
    }
  }
}

// Local Variables:
// mode: C++
// End:
