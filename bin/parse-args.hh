#ifndef VCSN_BIN_PARSE_ARGS_HH_
# define VCSN_BIN_PARSE_ARGS_HH_

# include <string>

# include <vcsn/algos/dyn.hh>

struct options
{
  bool is_automaton = true;
  std::string file;
  bool lal = true;
  std::string context = "char_b_lal";
  std::string labelset_describ = "abcd";
  vcsn::dyn::FileType input_format = vcsn::dyn::FileType::dotty;
  vcsn::dyn::FileType output_format = vcsn::dyn::FileType::dotty;
};

vcsn::dyn::FileType string_to_file_type(const std::string str);
options parse_args(int* argc, char* const * argv[]);
void usage(const char* prog, int exit_status);

#endif // !VCSN_BIN_PARSE_ARGS_HH_
