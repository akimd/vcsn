#include "parse-args.hh"

#include <getopt.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <stdexcept>

#include <vcsn/config.hh>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/stream.hh>

// FIXME: Factor in misc (see dyn/translate.hh).
static
std::string xgetenv(const char* var, const char* val)
{
  const char* cp = getenv(var);
  return cp ? cp : val;
}

options::options()
  : data_library(xgetenv("VCSN_DATA_PATH", VCSN_DATADIR), ":")
  , input_type(type::automaton)
{}

std::ostream& operator<<(std::ostream& o, const options& opts)
{
#define SHOW(F) o << #F ": " << opts.F << '\n'
  SHOW(program);
  SHOW(input);
  SHOW(input_is_file);
  SHOW(context);
  SHOW(output);
  SHOW(cmd);
  o << "argv:";
  for (const auto& a: opts.argv)
    o << ' ' << a;
  o << '\n';
#undef SHOW
  return o;
}

void options::print(bool a) const
{
  *out << a << std::endl;
}

void options::print(vcsn::dyn::automaton a) const
{
  *out << strip(a) << std::endl;
}

void options::print(vcsn::dyn::expression a) const
{
  *out << a << std::endl;
}

void options::print(vcsn::dyn::polynomial a) const
{
  *out << a << std::endl;
}

void options::print(vcsn::dyn::weight a) const
{
  *out << a << std::endl;
}

/// An istream for the input argv[0].
static
std::shared_ptr<std::istream>
input(const options& opts)
{
  if (opts.input_is_file)
    {
      auto p = opts.input == "-" || opts.input.empty()
        ? vcsn::path("-")
        : opts.data_library.find_file(opts.input);
      if (getenv("VCSN_DEBUG"))
        std::cerr << "reading: " << p << '\n';
      return vcsn::open_input_file(p.string());
    }
  else
    return std::make_shared<std::istringstream>(opts.input);
}

static
void check_eof(std::istream& is)
{
  if (is.peek() != -1)
    {
      std::string buf;
      is >> buf;
      throw std::domain_error("unexpected '" + vcsn::str_escape(buf)
                              + "', expected end of file");
    }
}

vcsn::dyn::automaton
read_automaton(const options& opts)
{
  auto is = input(opts);
  auto res = vcsn::dyn::read_automaton(*is, opts.input_format);
  check_eof(*is);
  return res;
}

vcsn::dyn::label
read_label(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  auto is = input(opts);
  auto res = vcsn::dyn::read_label(ctx, *is);
  check_eof(*is);
  return res;
}

vcsn::dyn::expression
read_expression(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  auto is = input(opts);
  auto res = vcsn::dyn::read_expression(ctx, {}, *is, opts.input_format);
  check_eof(*is);
  return res;
}

vcsn::dyn::polynomial
read_polynomial(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  auto is = input(opts);
  auto res = vcsn::dyn::read_polynomial(ctx, *is);
  check_eof(*is);
  return res;
}

vcsn::dyn::weight
read_weight(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  auto is = input(opts);
  auto res = vcsn::dyn::read_weight(ctx, *is);
  check_eof(*is);
  return res;
}

vcsn::dyn::label
read_word(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context);
  auto wctx = vcsn::dyn::make_word_context(ctx);
  auto is = input(opts);
  auto res = vcsn::dyn::read_label(wctx, *is);
  check_eof(*is);
  return res;
}

void
usage(const char* prog, int exit_status)
{
  if (exit_status == EXIT_SUCCESS)
    std::cout
      << "usage: " << prog << " [OPTIONS...] [ARGS...]\n"
      "\n"
      "General Options:\n"
      "  -h          display this help message and exit successfully\n"
      "  -v          display version information and exit successfully\n"
      "\n"
      "Input/Output:\n"
      "  -C CONTEXT    the context to use\n"
      "                'lal, b', 'law, q', 'lan(abc), zmin', etc.\n"
      "  -A            input is an automaton\n"
      "  -E            input is a rational expression\n"
      "  -P            input is a polynomial\n"
      "  -W            input is a weight\n"
      "  -e STRING     input is STRING\n"
      "  -f FILE       input is FILE\n"
      "  -I FORMAT     input format (dot, efsm, fado, text)\n"
      "  -O FORMAT     output format\n"
      "                (dot, efsm, grail, info, list, null, text, tikz, utf8)\n"
      "  -o FILE       save output into FILE\n"
      "  -q            discard any output\n"
      "\n"
      "Input/Output Formats (for Automata, Expressions, Polynomials, Weights):\n"
      "  dot    A     GraphViz's Dot language\n"
      "  efsm   A     Extended FSM format for OpenFST: use efstcompile\n"
      "  fado   A     FAdo's format\n"
      "  grail  A     Grail's format\n"
      "  info   AE    facts about the result (size, etc.)\n"
      "  latex   EPW  display as a LaTeX formula\n"
      "  list     P   display one monomial per line\n"
      "  null   AEPW  no output at all (e.g., for benchmarks)\n"
      "  text    EPW  usual concrete syntax in ASCII\n"
      "  tikz   A     LaTeX source for TikZ\n"
      "  utf8    EPW  usual concrete syntax in UTF-8\n"
      "\n"
      "Examples:\n"
      "  $ vcsn thompson -Ee '[ab]*a[ab]{3}' |\n"
      "      vcsn proper |\n"
      "      vcsn determinize |\n"
      "      vcsn evaluate 'abba'\n"
      "\n"
      "  $ vcsn thompson -Ee '[ab]*a[ab]{3}' |\n"
      "      vcsn proper -f - |\n"
      "      vcsn determinize -f - |\n"
      "      vcsn evaluate -f - 'abba'\n"
      "\n"
      "  $ vcsn derived-term -C 'lat<lan, lan>, q' -Ee 'a*|b*' |\n"
      "      vcsn shortest 10\n"
      ;
  else
    std::cerr << "Try `" << prog << " -h' for more information.\n";
  exit(exit_status);
}

void
version(const options& opts)
{
  std::cout
    << opts.program << " (" VCSN_PACKAGE_STRING ")\n"
    << "<" VCSN_PACKAGE_URL ">\n"
    << "\n"
#define DEFINE(Var)                             \
    << #Var ": " VCSN_ ## Var "\n"
    DEFINE(CXX)
    DEFINE(CPPFLAGS)
    DEFINE(CXXFLAGS)
    DEFINE(DATADIR)
#undef DEFINE
    << "Data library: " << opts.data_library << '\n'
    ;
  exit(EXIT_SUCCESS);
}

void
parse_args(options& opts, int argc, char* const* argv)
{
  if (opts.program.empty())
    opts.program = vcsn::path(argv[0]).filename().string();

  vcsn::require(1 < argc, "no command given");
  opts.cmd = argv[1];
  argc -= 1;
  argv += 1;
  if (opts.cmd == "--help" || opts.cmd == "-h")
    opts.cmd = "help";
  if (opts.cmd == "--version" || opts.cmd == "-v")
    opts.cmd = "version";

  int opt;
  while ((opt = getopt(argc, argv, "AC:Ee:f:hI:O:o:PqvW?")) != -1)
    switch (opt)
      {
      case 'A':
        opts.input_type = type::automaton;
        break;
      case 'C':
        opts.context = optarg;
        break;
      case 'E':
        opts.input_type = type::expression;
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
      case 'P':
        opts.input_type = type::polynomial;
        break;
      case 'q':
        opts.output_format = "null";
        break;
      case 'v':
        version(opts);
        break;
      case 'W':
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
parse_args(int argc, char* const* argv)
{
  options res;
  parse_args(res, argc, argv);
  if (getenv("VCSN_DEBUG"))
    std::cerr << res << '\n';
  return res;
}
