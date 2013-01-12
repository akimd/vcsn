#ifndef VCSN_BIN_PARSE_ARGS_HH_
# define VCSN_BIN_PARSE_ARGS_HH_

# include <string>

# include <vcsn/algos/dyn.hh>

struct options
{
  bool is_automaton = true;
  std::string input;
  bool input_is_file = true;
  bool lal = true;
  std::string context = "lal_char_b";
  std::string labelset_describ = "abcd";
  vcsn::dyn::FileType input_format = vcsn::dyn::FileType::dot;
  vcsn::dyn::FileType output_format = vcsn::dyn::FileType::dot;
};

vcsn::dyn::FileType string_to_file_type(const std::string str);
void usage(const char* prog, int exit_status);

/// Read the command line arguments.
void parse_args(options& opts, int& argc, char* const*& argv);
options parse_args(int& argc, char* const*& argv);

vcsn::dyn::automaton read_automaton(const options& opts);
vcsn::dyn::ratexp read_ratexp(const options& opts);

#endif // !VCSN_BIN_PARSE_ARGS_HH_
