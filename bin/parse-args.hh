#ifndef VCSN_BIN_PARSE_ARGS_HH_
# define VCSN_BIN_PARSE_ARGS_HH_

# include <string>

# include <vcsn/dyn/algos.hh>

struct options
{
  bool is_automaton = true;
  std::string input;
  bool input_is_file = true;
  std::string output = "-";
  bool lal = true;
  std::string context = "lal_char(abcd)_b";
  vcsn::dyn::FileType input_format = vcsn::dyn::FileType::dot;
  vcsn::dyn::FileType output_format = vcsn::dyn::FileType::dot;
};

vcsn::dyn::FileType string_to_file_type(const std::string str);
void usage(const char* prog, int exit_status);

/// Read the command line arguments.
void parse_args(options& opts, int& argc, char* const*& argv);
options parse_args(int& argc, char* const*& argv);

/// Read automaton/ratexp according to \a opts.
vcsn::dyn::automaton read_automaton(const options& opts);
vcsn::dyn::ratexp read_ratexp(const options& opts);

/// Print automaton/ratexp according to \a opts.
void print(const options& opts, const vcsn::dyn::automaton& exp);
void print(const options& opts, const vcsn::dyn::ratexp& exp);


#endif // !VCSN_BIN_PARSE_ARGS_HH_
