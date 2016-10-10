#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/stream.hh> // get_file_contents().

#include "vcsn-tools.hh"

#pragma GCC diagnostic ignored "-Wmissing-declarations"

using namespace vcsn;
using namespace vcsn::tools;

namespace {
  int print_usage(std::string);
  int list_commands();

  bool used_stdin = false;

  std::string read_stdin()
  {
    used_stdin = true;
    std::cin >> std::noskipws;
    std::istreambuf_iterator<char> begin(std::cin), end;
    return std::string(begin, end);
  }

  struct options
  {
    std::vector<parsed_arg> args;
    std::string output_file;
    std::string output_format;
    std::string context_string;
  };

  options parse_arguments(int argc, char** argv)
  {
    // We are not interested by the program or the function name.
    --argc;
    ++argv;

    auto res
      = options{std::vector<parsed_arg>{}, "", "default", "lal_char, b"};

    auto t = type::unknown;
    std::string input_format = "default";

    const char optstring[] =
#ifdef __GNU_LIBRARY__
      // Put GNU getopt() in POSIXLY_CORRECT mode.
      "+"
#endif
      "ABDEFLNPSWchf:e:C:O:o:qI:";

    while (true)
      switch (char opt = getopt(argc, argv, optstring))
      {
        case 'f':
          if (optarg == std::string("-"))
            res.args.push_back({read_stdin(), t, input_format});
          else
            res.args.push_back({get_file_contents(optarg), t, input_format});
          t = type::unknown;
          input_format = "default";
          break;

        case 'O':
         res.output_format = optarg;
         break;

        case 'I':
         if (input_format != "default")
           raise("too many input formats for one argument");
         else
           input_format = optarg;
         break;

        case 'o':
          res.output_file = optarg;
          break;

        case 'q':
          res.output_file = "/dev/null";
          break;

        case -1:
          if (optind == argc)
            return res;
          else if (!strcmp(argv[optind], "-")) //We need to read stdin.
            {
              res.args.push_back({read_stdin(), t, input_format});
              t = type::unknown;
              input_format = "default";
            }
          else
            {
              res.args.push_back({std::string(argv[optind]), t, input_format});
              t = type::unknown;
              input_format = "default";
            }
          optind++;
          break;

        case 'e':
          res.args.push_back({optarg, t, input_format});
          t = type::unknown;
          input_format = "default";
          break;

        case 'C':
          res.context_string = optarg;
          break;

        case 'A':
        case 'B':
        case 'D':
        case 'E':
        case 'F':
        case 'L':
        case 'N':
        case 'P':
        case 'S':
        case 'W':
          // This cast is safe as long as the
          // case list is properly maintened.
          if (t == type::unknown)
            t = static_cast<type>(opt);
          else
            raise("too many type qualifiers for one argument");
          break;

        case 'c':
          list_commands();
          exit(0);

        case 'h':
          print_usage(argv[0]);
          exit(0);
          
        case '?':
          //ERROR
          raise("unknown option: ", optarg);
      }
  }

  bool is_match(const algo& a, const std::vector<parsed_arg>& args)
  {
    if (a.signature.size() != args.size())
      return false;

    for (size_t i = 0; i < args.size(); i++)
      {
        if (args[i].t != a.signature[i] && args[i].t != type::unknown)
          return false;
      }
    return true;
  }

  const algo* match(const std::string& algo_name,
                    std::vector<parsed_arg>& args,
                    const dyn::context& context)
  {
    const algo* a = nullptr;
    auto range = vcsn::tools::algos.equal_range(algo_name);
    for (auto it = range.first; it != range.second; it++)
      {
        const auto& candidate = it->second;
        if (!is_match(candidate, args))
          continue;
        else if (a)
          {
            std::ostringstream ss;
            ss << "more than one algorithm found.\n"
               << "candidates are:\n";
            for (auto it = range.first; it != range.second; it++)
              if(is_match(it->second, args))
                ss << "  "<< it->second.declaration << '\n';
            raise(ss.str());
          }
        else
          a = &candidate;
      }
    return a;
  }

  /// Try to match for an algorithm.
  void match_and_call(const std::string& algo_name,
                      std::vector<parsed_arg>& args,
                      const dyn::context& context)
  {
    auto a = match(algo_name, args, context);
    if (!a && !used_stdin)
      {
        // Let's try with stdin as an input.
        args.insert(args.begin(), {"", type::unknown, "default"});
        a = match(algo_name, args, context);
        if (a) // We found an algo, time to read stdin
          args[0].arg = read_stdin();
      }
    if (!a)
      {
        std::ostringstream ss;
        ss << algo_name << ": no matching algorithm\n"
           << "  candidates are:\n";
        auto range = algos.equal_range(algo_name);
        for (auto it = range.first; it != range.second; it++)
          ss << "    " << it->second.declaration << '\n';
        raise(ss.str());
      }
    else
      a->exec(args, context);
  }

  int
  list_commands()
  {
    // The list of algorithm names.
    auto names = std::set<std::string>{};
    std::transform(begin(vcsn::tools::algos), end(vcsn::tools::algos),
                   std::inserter(names, begin(names)),
                   [](const auto& a)
                   {
                     auto name = a.first;
                     std::replace(name.begin(), name.end(), '_', '-');
                     return name;
                   });

    // Pretty-print it.
    constexpr size_t max_width = 70;
    size_t space_left = max_width - 2;
    std::cout << ' ';
    for (const auto& a : names)
      if (space_left < (a.size()+1))
        {
          std::cout << "\n  " << a;
          space_left = max_width - (a.size() + 2);
        }
      else
        {
          std::cout << ' ' << a ;
          space_left -= a.size() + 1;
        }

    std::cout << '\n';
    return 0;
  }

  int print_usage(std::string algo_name = "")
  {
    std::cout <<
      "usage: vcsn "<< (algo_name == "" ? "COMMAND" : algo_name) << " [OPTIONS...] [ARGS...]\n"
      "\n";
    if (!algo_name.empty())
    {
      std::cout << "Available versions:\n";
      auto range = vcsn::tools::algos_doc.equal_range(algo_name);
      for (auto it = range.first; it != range.second; it++)
        std::cout << it->second.declaration << '\n'
                  << "  " << it->second.doc << "\n\n";
      std::cout << "For more help about available options, please use \"vcsn tools --help\"\n";
    }
    else
    {
      std::cout <<
        "General Options:\n"
        "  -h, --help      display this help message and exit successfully\n"
        "  -c, --commands  display the supported commands and exit successfully\n"
        "\n"
        "Available COMMANDs:\n";
      list_commands();
      std::cout <<
         "\n"
         "Input/Output:\n"
         "  -C CONTEXT    the context to use\n"
         "                'lal, b', 'law, q', 'lan(abc), zmin', etc.\n"
         "  -A            input is an automaton\n"
         "  -B            input is a boolean\n"
         "  -D            input is an identity\n"
         "  -E            input is a rational expression\n"
         "  -F            input is a float\n"
         "  -L            input is a label (or a word)\n"
         "  -N            input is a number\n"
         "  -P            input is a polynomial\n"
         "  -S            input is a string\n"
         "  -W            input is a weight\n"
         "  -e STRING     input is STRING\n"
         "  -f FILE       input is FILE\n"
         "  -I FORMAT     input format (daut, dot, efsm, fado, text)\n"
         "  -O FORMAT     output format\n"
         "                (daut, dot, efsm, grail, info, list, null, text, tikz, utf8)\n"
         "  -o FILE       save output into FILE\n"
         "  -q            discard any output\n"
         "\n"
         "Input/Output Formats (for Automata, Expressions, Labels, Polynomials, Weights):\n"
         "  daut   A      Simplified Dot syntax for Automata\n"
         "  dot    A      GraphViz's Dot language\n"
         "  efsm   A      Extended FSM format for OpenFST: use efstcompile\n"
         "  fado   A      FAdo's format\n"
         "  grail  A      Grail's format\n"
         "  info   AE     facts about the result (size, etc.)\n"
         "  latex   ELPW  display as a LaTeX formula\n"
         "  list      P   display one monomial per line\n"
         "  null   AELPW  no output at all (e.g., for benchmarks)\n"
         "  text    ELPW  usual concrete syntax in ASCII\n"
         "  tikz   A      LaTeX source for TikZ\n"
         "  utf8    ELPW  usual concrete syntax in UTF-8\n"
         "\n"
         "Examples:\n"
         "  $ vcsn thompson -Ee '[ab]*a[ab]{3}' |\n"
         "      vcsn proper |\n"
         "      vcsn determinize |\n"
         "      vcsn eval -L 'abba'\n"
         "\n"
         "  $ vcsn thompson -Ee '[ab]*a[ab]{3}' |\n"
         "      vcsn proper -f - |\n"
         "      vcsn determinize -f - |\n"
         "      vcsn eval -f - -L 'abba'\n"
         "\n"
         "  $ vcsn derived-term -C 'lat<lan, lan>, q' -Ee 'a*|b*' |\n"
         "      vcsn shortest 10\n";
    }
    return 0;
  }
}

int main(int argc, char** argv)
try
{
  if (argc < 2)
    return print_usage();

  auto algo = std::string(argv[1]);
  if (algo == "--commands" || algo == "commands" || algo == "-c")
    return list_commands();
  if (algo == "-h" || algo == "--help") 
    return print_usage();

  std::replace(algo.begin(), algo.end(), '-', '_');
  vcsn::require(algos.find(algo) != end(algos),
                "unknown algorithm: ", argv[1]);

  if (argc > 2 && (argv[2] == std::string("--help") || argv[2] == std::string("-h")))
    return print_usage(algo);

  auto options = parse_arguments(argc, argv);

  auto out = std::shared_ptr<std::ostream>{};
  auto saved_cout_buffer = std::cout.rdbuf();
  if (!options.output_file.empty())
    {
      // Redirect std::cout.
      if (out = open_output_file(options.output_file))
        std::cout.rdbuf(out->rdbuf());
      else
        // FIXME: strerror
        raise("can't open output file: ", options.output_file);
    }

  dyn::set_format(std::cout, options.output_format);
  auto ctx = dyn::make_context(options.context_string);
  match_and_call(argv[1], options.args, ctx);
  std::cout << '\n';

  // Reattach std::cout to the standard output.
  std::cout.rdbuf(saved_cout_buffer);
}
catch (const std::exception& e)
{
  std::cerr << "error: " << e.what() << '\n';
  exit(EXIT_FAILURE);
}
catch (...)
{
  std::cerr << "error: unknown exception caught\n";
  exit(EXIT_FAILURE);
}
