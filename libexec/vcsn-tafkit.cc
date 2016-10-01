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

      auto res = options{std::vector<parsed_arg>{}, "", "default", "lal_char, b"};

      auto t = type::unknown;
      std::string input_format = "default";

      const char optstring[] =
#ifdef __GNU_LIBRARY__
        // Put GNU getopt() in POSIXLY_CORRECT mode.
        "+"
#endif
        "ALEPWSBFUf:e:C:O:o:I:";

      while (true)
        switch(char opt = getopt(argc, argv, optstring))
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
           if(input_format != "default")
             {
               std::cerr << "Too many input formats for one argument\n";
               exit(1);
             }
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
              {
                std::cout << "Too many type qualifiers for one argument\n";
                exit(1);
              }
            break;

          case '?':
            //ERROR
            std::cerr << "Unknown option: " << opt << '\n';
            exit(1);
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

    const algo* match(
        const std::string& algo_name,
        std::vector<parsed_arg>& args,
        dyn::context context)
    {
      const algo* a = nullptr;
      auto range = algos.equal_range(algo_name);
      for (auto it = range.first; it != range.second; it++)
        {
          const auto& candidate = it->second;
          if (!is_match(candidate, args))
            continue;
          else if (a)
            {
              //TODO: make a better error.
              std::cout << "More than one algo found\n";
              std::exit(1);
            }
          else
            {
              a = &candidate;
            }
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
          for (auto arg : args)
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
    // Extract the algorithm list into v.
    auto v = std::vector<std::string>{};
    for (auto it = vcsn::tafkit::algos.begin();
         it != vcsn::tafkit::algos.end();
         it = vcsn::tafkit::algos.equal_range(it->first).second)
      {
        auto algo = it->first;
        std::replace(algo.begin(), algo.end(), '_', '-');
        v.emplace_back(algo);
      }

    // Sort it.
    std::sort(v.begin(), v.end());

    // Pretty-print it.
    constexpr size_t max_width = 70;
    size_t space_left = max_width - 2;
    std::cout << "  ";
    for (auto a : v)
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
{
  if (argv[1] == std::string("--commands"))
    return list_commands();

  auto algo = std::string(argv[1]);
  std::replace(algo.begin(), algo.end(), '-', '_');

  auto options = parse_arguments(argc, argv);

  std::shared_ptr<std::ostream> out;
  auto saved_cout_buffer = std::cout.rdbuf();
  if (options.output_file != "")
    {
      // Redirect std::cout.
      if (out = open_output_file(options.output_file))
        std::cout.rdbuf(out->rdbuf());
      else
        {
          std::cerr << "Can't open output file\n";
          exit(1);
        }
    }

  dyn::set_format(std::cout, options.output_format);
  auto ctx = dyn::make_context(options.context_string);
  match_and_call(argv[1], options.args, ctx);
  std::cout << '\n';

  // Reattach std::cout to the standard output.
  std::cout.rdbuf(saved_cout_buffer);

  return 0;
}
