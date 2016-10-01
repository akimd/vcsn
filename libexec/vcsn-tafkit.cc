#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/stream.hh> // get_file_contents().

#include "vcsn-tafkit.hh"

#pragma GCC diagnostic ignored "-Wmissing-declarations"

namespace vcsn
{
  namespace tafkit
  {
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
        "ALEPWSBFUf:e:C:O:o:I:";

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

          case '?':
            //ERROR
            raise("unknown option: ", opt);
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
                      const std::vector<parsed_arg>& args,
                      dyn::context context)
    {
      const algo* a = nullptr;
      auto range = vcsn::tafkit::algos.equal_range(algo_name);
      for (auto it = range.first; it != range.second; it++)
        {
          const auto& candidate = it->second;
          if (!is_match(candidate, args))
            continue;
          else if (a)
            // FIXME: make a better error.
            raise("more than one algo found");
          else
            a = &candidate;
        }
      return a;
    }

    // Try to match for an algorithm.
    void match_and_call(const std::string& algo_name,
                        std::vector<parsed_arg>& args,
                        const dyn::context& context)
    {
      auto a = match(algo_name, args, context);
      if (!a && !used_stdin)
        {
          //Let's try with stdin as an input.
          args.insert(args.begin(), {read_stdin(), type::unknown, "default"});
          a = match(algo_name, args, context);
        }
      if (!a)
        {
          //TODO: make a better error.
          std::cerr << "No algo found\n";
          for (const auto& arg : args)
            // Type is a char-based enum class, this cast is safe.
            std::cerr << static_cast<char>(arg.t) << '\n';
          std::exit(1);
        }
      else
        a->exec(args, context);
    }
  }
}

namespace
{
  int
  list_commands()
  {
    // The list of algorithm names.
    auto names = std::set<std::string>{};
    std::transform(begin(vcsn::tafkit::algos), end(vcsn::tafkit::algos),
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
    std::cout << "  ";
    for (const auto& a : names)
      {
        if (space_left < a.size() + 1)
          {
            std::cout << "\n  ";
            space_left = max_width - 2;
          }

        std::cout << a << ' ';
        space_left -= a.size() + 1;
      }

    std::cout << '\n';
    return 0;
  }
}

using namespace vcsn;
using namespace vcsn::tafkit;
int main(int argc, char** argv)
try
{
  auto algo = std::string(argv[1]);
  if (algo == "--commands" || algo == "commands")
    return list_commands();
  std::replace(algo.begin(), algo.end(), '-', '_');
  vcsn::require(algos.find(algo) != end(algos),
                "unknown algorithm: ", argv[1]);

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
