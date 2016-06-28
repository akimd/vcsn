#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/file-library.hh>

enum class type
  {
    automaton,
    polynomial,
    expression,
    weight,
  };

struct options
{
  options();

  /// The name of this program (argv[0]).
  std::string program;

  /// The search path for data files.
  vcsn::file_library data_library;

  /// The type of input.
  type input_type;
  /// Whether \a input is a file name, or a value.
  bool input_is_file = true;
  /// The input (name or value).
  std::string input;
  /// Input format.
  std::string input_format = "default";

  /// Context.
  std::string context = "lal_char, b";

  /// Output file name ("-" for stdout).
  std::string output = "-";
  /// Output format.
  std::string output_format = "default";

  /// The command.
  std::string cmd;

  /// The remainder of the arguments.
  std::vector<std::string> argv;

  /// The output stream.
  std::shared_ptr<std::ostream> out;

  void print(bool a) const;
  void print(vcsn::dyn::automaton a) const;
  void print(vcsn::dyn::expression a) const;
  void print(vcsn::dyn::polynomial a) const;
  void print(vcsn::dyn::weight a) const;
};

std::ostream& operator<<(std::ostream& o, const options& opts);

/// The supported commands.
const char* commands();
void usage(const char* prog, int exit_status);
void version(const options& opts);

/// Read the command line arguments.
void parse_args(options& opts, int argc, char* const* argv);
options parse_args(int argc, char* const* argv);

/// Read automaton/expression according to \a opts.
vcsn::dyn::automaton read_automaton(const options& opts);
vcsn::dyn::label read_label(const options& opts);
vcsn::dyn::expression read_expression(const options& opts);
vcsn::dyn::polynomial read_polynomial(const options& opts);
vcsn::dyn::weight read_weight(const options& opts);
vcsn::dyn::label read_word(const options& opts);

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

  virtual int work_polynomial(const options&) const
  {
    throw std::runtime_error("not implemented for polynomials");
  }

  virtual int work_weight(const options&) const
  {
    throw std::runtime_error("not implemented for weights");
  }
};
