#ifndef VCSN_BIN_PARSE_ARGS_HH_
# define VCSN_BIN_PARSE_ARGS_HH_

# include <string>
# include <vector>

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
  std::string input_format = "dot";

  /// Whether input is LAL (obsolete, FIXME: remove).
  bool lal = true;
  /// Context.
  std::string context = "lal_char(abcd)_b";

  /// Output file name ("-" for stdout).
  std::string output = "-";
  /// Output format.
  std::string output_format = "default";

  /// The remainder of the arguments.
  std::vector<std::string> argv;
};

[[noreturn]]
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

/// Function object to dispatch calls for automata or rational expressions.
struct vcsn_function
{
  virtual int work_aut(const options&) const
  {
    throw std::runtime_error("not implemented for automata");
  }

  virtual int work_exp(const options&) const
  {
    throw std::runtime_error("not implemented for rational expressions");
  }
};

/// Read the command line argument, and based on them, run \a fun's \a
/// work_aut, or \a work_exp.
/// \param argc         argv size
/// \param argv         command line arguments
/// \param fun          the visitor to run
/// \param is_automaton whether if not specified, input is an automaton.
int vcsn_main(int argc, char* const argv[], const vcsn_function& fun,
              bool is_automaton = true);

#endif // !VCSN_BIN_PARSE_ARGS_HH_
