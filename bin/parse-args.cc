#include <getopt.h>

#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>

// Temporary.
#include <boost/algorithm/string/replace.hpp>

#include "parse-args.hh"
#include <vcsn/dyn/algos.hh>

vcsn::dyn::FileType
string_to_file_type(const std::string str)
{
  using vcsn::dyn::FileType;
  if (str == "dot")
    return FileType::dot;
  else if (str == "fsm")
    return FileType::fsm;
  else if (str == "null")
    return FileType::null;
  else if (str == "text")
    return FileType::text;
  else if (str == "xml")
    return FileType::xml;
  else
    throw std::domain_error("invalid file type: " + str);
}

vcsn::dyn::automaton
read_automaton(const options& opts)
{
  return
    opts.input_is_file
    ? vcsn::dyn::read_automaton_file(opts.input)
    : vcsn::dyn::read_automaton_string(opts.input);
}

vcsn::dyn::ratexp
read_ratexp(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  // Be cool, we don't support many formats.
  vcsn::dyn::FileType fmt
    = (opts.output_format == vcsn::dyn::FileType::null
       || opts.output_format == vcsn::dyn::FileType::text
       ? opts.output_format
       : vcsn::dyn::FileType::text);
  return
    opts.input_is_file
    ? vcsn::dyn::read_ratexp_file(opts.input, ctx, fmt)
    : vcsn::dyn::read_ratexp_string(opts.input, ctx, fmt);
}

void
print(const options& opts, const vcsn::dyn::automaton& aut)
{
  if (opts.output_format != vcsn::dyn::FileType::null)
    {
      std::ostream* out = &std::cout;
      std::ofstream os;
      if (!opts.output.empty() && opts.output != "-")
        {
          os.open(opts.output.c_str());
          out = &os;
        }
      if (!out->good())
        throw std::runtime_error(opts.output + ": cannot open for writing");

      print(aut, *out, opts.output_format) << std::endl;
    }
}

void
print(const options& opts, const vcsn::dyn::ratexp& exp)
{
  if (opts.output_format != vcsn::dyn::FileType::null)
    {
      std::ostream* out = &std::cout;
      std::ofstream os;
      if (!opts.output.empty() && opts.output != "-")
        {
          os.open(opts.output.c_str());
          out = &os;
        }
      if (!out->good())
        throw std::runtime_error(opts.output + ": cannot open for writing");

      // Be cool, we don't support many formats.
      print(exp, *out, vcsn::dyn::FileType::text) << std::endl;
    }
}

void
usage(const char* prog, int exit_status)
{
  if (exit_status == EXIT_SUCCESS)
    std::cout
      << "usage: " << prog << " [OPTIONS...]\n"
      "\n"
      "Options:\n"
      "  -A            input is an automaton\n"
      "  -E            input is a rational expression\n"
      "  -e STRING     input is STRING\n"
      "  -f FILE       input is FILE\n"
      "  -I FORMAT     input format (dot, text, xml)\n"
      "  -O FORMAT     output format (dot, text, xml)\n"
      "  -o FILE       save output into FILE\n"
      "\n"
      "Context:\n"
      "  -C CONTEXT         the context to use\n"
      "                     la(l|u|w)_char_(b|z|zmin), etc.\n"
      "  -L letters|words   kind of the labels\n"
      "  -W WEIGHT-SET      define the kind of the weights\n"
      "  -g STRING          generator set definition\n"
      "\n"
      "WeightSet:\n"
      "  b        for Boolean\n"
      "  br       for RatExp<b>\n"
      "  z        for Z\n"
      "  zr       for RatExp<Z>\n"
      "  zrr      for Ratexp<RatExp<Z>>\n"
      ;
  else
    std::cerr << "Try `" << prog << " -h' for more information."
              << std::endl;
  exit(exit_status);
}

// Convert "w" to "l" for "brutal" assembled context names.
void
apply_label_kind(options& opts)
{
  using boost::algorithm::replace_all;
  if (opts.lal)
    replace_all(opts.context, "law", "lal");
  else
    replace_all(opts.context, "lal", "law");
}

void
parse_args(options& opts, int& argc, char* const*& argv)
{
  using map = std::map<std::string, std::string>;
  using pair = std::pair<std::string, std::string>;

  int opt;
  map ksets;
#define ADD(Key, Name)                          \
    ksets.insert(pair(#Key, Name))
    ADD(b,   "law_char(abcd)_b");
    ADD(br,  "law_char(abcd)_ratexpset<law_char(efgh)_b>");
    ADD(z,   "law_char(abcd)_z");
    ADD(zr,  "law_char(abcd)_ratexpset<law_char(efgh)_z>");
    ADD(zrr, "law_char(abcd)_ratexpset<law_char(efgh)_ratexpset<law_char(xyz)_z>>");
#undef ADD
  while ((opt = getopt(argc, argv, "AC:Ee:f:hI:L:O:o:W:?")) != -1)
    switch (opt)
      {
      case 'A':
        opts.is_automaton = true;
        opts.input_format = vcsn::dyn::FileType::dot;
        break;
      case 'C':
        opts.context = optarg;
        break;
      case 'E':
        opts.is_automaton = false;
        opts.input_format = vcsn::dyn::FileType::text;
        break;
      case 'e':
        opts.input = optarg;
        opts.input_is_file = false;
        break;
      case 'f':
        opts.input = optarg;
        opts.input_is_file = true;
        break;
      case 'h':
        usage(argv[0], EXIT_SUCCESS);
        break;
      case 'I':
        opts.input_format = string_to_file_type(optarg);
        break;
      case 'O':
        opts.output_format = string_to_file_type(optarg);
        break;
      case 'o':
        opts.output = optarg;
        break;
      case 'L':
        {
          std::string s = optarg;
          if (s == "l" || s == "letter" || s == "letters")
            opts.lal = true;
          else if (s == "w" || s == "word" || s == "words")
            opts.lal = false;
          else
            {
              std::cerr << optarg << ": invalid label kind (-L)" << std::endl;
              goto fail;
            }
          apply_label_kind(opts);
          break;
        }
      case 'W':
        {
          map::iterator i = ksets.find(optarg);
          if (i == end(ksets))
            {
              std::cerr << optarg << ": invalid weight set (-W)" << std::endl;
              goto fail;
            }
          else
            {
              opts.context = i->second;
              apply_label_kind(opts);
            }
          break;
        }
      case '?':
      fail:
        usage(argv[0], EXIT_FAILURE);
        break;
      }
  argc -= optind;
  argv += optind;
}

options
parse_args(int& argc, char* const*& argv)
{
  options res;
  parse_args(res, argc, argv);
  return res;
}
