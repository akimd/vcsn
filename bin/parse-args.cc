#include <getopt.h>

#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>

// Temporary.
#include <boost/algorithm/string/replace.hpp>

#include "parse-args.hh"
#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/stream.hh>

vcsn::dyn::automaton
read_automaton(const options& opts)
{
  return
    opts.input_is_file
    ? vcsn::dyn::read_automaton_file(opts.input, opts.input_format)
    : vcsn::dyn::read_automaton_string(opts.input);
}

vcsn::dyn::ratexp
read_ratexp(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  // Be cool, we don't support many formats.
  std::string fmt
    = (opts.input_format == "null" || opts.input_format == "text"
       ? opts.input_format
       : "text");
  return
    opts.input_is_file
    ? vcsn::dyn::read_ratexp_file(opts.input, ctx, fmt)
    : vcsn::dyn::read_ratexp_string(opts.input, ctx, fmt);
}

vcsn::dyn::weight
read_weight(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  return
    opts.input_is_file
    ? vcsn::dyn::read_weight_file(opts.input, ctx)
    : vcsn::dyn::read_weight_string(opts.input, ctx);
}

void
usage(const char* prog, int exit_status)
{
  if (exit_status == EXIT_SUCCESS)
    std::cout
      << "usage: " << prog << " [OPTIONS...] [ARGS...]\n"
      "\n"
      "Input/Output:\n"
      "  -C CONTEXT    the context to use\n"
      "                la[lnow]_char_(b|q|r|z|zmin), etc.\n"
      "  -A            input is an automaton\n"
      "  -E            input is a rational expression\n"
      "  -w            input is a weight\n"
      "  -e STRING     input is STRING\n"
      "  -f FILE       input is FILE\n"
      "  -I FORMAT     input format (dot, fado, text)\n"
      "  -O FORMAT     output format (dot, efsm, grail, info, list, null, text, tikz)\n"
      "  -o FILE       save output into FILE\n"
      "  -q            discard any output\n"
      "\n"
      "Input/Output Formats (for Automata, Expressions, Polynomials, Weights):\n"
      "  dot    A     GraphViz's Dot language\n"
      "  efsm   A     Extended FSM format for OpenFST: use efstcompile\n"
      "  fado   A     FAdo's format\n"
      "  grail  A     Grail's format\n"
      "  info   AE    facts about the result (size, etc.)\n"
      "  list     P   display one monomial per line\n"
      "  null   AEPW  no output at all (e.g., for benchmarks)\n"
      "  text    EPW  usual concrete syntax\n"
      "  tikz   A     LaTeX source for TikZ\n"
      ;
  else
    std::cerr << "Try `" << prog << " -h' for more information."
              << std::endl;
  exit(exit_status);
}

void
parse_args(options& opts, int& argc, char* const*& argv)
{
  if (opts.program.empty())
    opts.program = argv[0];

  int opt;
  while ((opt = getopt(argc, argv, "AC:Ee:f:hI:O:o:qw?")) != -1)
    switch (opt)
      {
      case 'A':
        opts.input_type = type::automaton;
        break;
      case 'C':
        opts.context = optarg;
        break;
      case 'E':
        opts.input_type = type::ratexp;
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
        opts.input_format = optarg;
        break;
      case 'O':
        opts.output_format = optarg;
        break;
      case 'o':
        opts.output = optarg;
        break;
      case 'q':
        opts.output_format = "null";
        break;
      case 'w':
        opts.input_type = type::weight;
        break;
      case '?':
        usage(argv[0], EXIT_FAILURE);
        break;
      }
  argc -= optind;
  argv += optind;
  opts.argv.insert(opts.argv.end(), argv, argv + argc);
  // Open the file anyway, as the user specified it.
  opts.out = vcsn::open_output_file(opts.output);
  // With -O null, output nothing, not even the `\n' after the result.
  if (opts.output_format == "null")
    opts.out.reset(&vcsn::cnull, [](...){});
  vcsn::dyn::set_format(*opts.out, opts.output_format);
  *opts.out << std::boolalpha;
}

options
parse_args(int& argc, char* const*& argv)
{
  options res;
  parse_args(res, argc, argv);
  return res;
}


int
vcsn_main(int argc, char* const argv[], const vcsn_function& fun,
          type t)
{
  options opts;
  opts.input_type = t;
  try
    {
      parse_args(opts, argc, argv);
      switch (opts.input_type)
        {
        case type::automaton: return fun.work_aut(opts);
        case type::ratexp:    return fun.work_exp(opts);
        case type::weight:    return fun.work_weight(opts);
        }
      abort();
    }
  catch (const std::exception& e)
    {
      std::cerr << opts.program << ": " << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  catch (...)
    {
      std::cerr << opts.program << ": unknown exception caught" << std::endl;
      exit(EXIT_FAILURE);
    }
}
