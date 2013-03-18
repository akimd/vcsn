#ifndef VCSN_BIN_PARSE_ARGS_HH_
# define VCSN_BIN_PARSE_ARGS_HH_

# include <string>

# include <vcsn/dyn/algos.hh>

struct options
{
  /// The name of this program (argv[0]).
  std::string program;

  /// Whether the input is about an automaton.
  bool is_automaton = true;
  /// Whether \a input is a file name, or a value.
  bool input_is_file = true;
  /// The input (name or value).
  std::string input;
  /// Input format.
  vcsn::dyn::FileType input_format = vcsn::dyn::FileType::dot;

  /// Whether input is LAL (obsolete, FIXME: remove).
  bool lal = true;
  /// Context.
  std::string context = "lal_char(abcd)_b";

  /// Output file name ("-" for stdout).
  std::string output = "-";
  /// Output format.
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
