#include <getopt.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "parse-args.hh"

vcsn::dyn::FileType
string_to_file_type(const std::string str)
{
  using vcsn::dyn::FileType;
  if (str == "dotty")
    return FileType::dotty;
  else if (str == "text")
    return FileType::text;
  else if (str == "xml")
    return FileType::xml;
  else
    throw std::domain_error(str + " is an invalid file type.");
}

void
usage(const char* prog, int exit_status)
{
  if (exit_status == EXIT_SUCCESS)
    std::cout
      << "usage: " << prog << " [OPTIONS...] FILE\n"
      "\n"
      "Context:\n"
      "  -a                 FILE contains an automaton\n"
      "  -e                 FILE contains an expression\n"
      "  -L letter|words    kind of the labels\n"
      "  -W WEIGHT-SET      define the kind of the weights\n"
      "  -g STRING          generator set definition\n"
      "Format:\n"
      "  -i FORMAT          input format\n"
      "  -o FORMAT          output format\n"
      "WEIGHT-SET:\n"
      "  b        for Boolean\n"
      "  br       for RatExp<b>\n"
      "  z        for Z\n"
      "  zr       for RatExp<Z>\n"
      "  zrr      for Ratexp<RatExp<Z>>\n"
      ;
  else
    std::cerr << "Try `" << prog << " -h' for more information."
              << std::endl;
  exit(exit_status);
}

options
parse_args(int& argc, char* const*& argv)
{
  const char* prog = argv[0];
  using map = std::map<std::string, std::string>;
  using pair = std::pair<std::string, std::string>;

  options opts;
  int opt;
  map ksets;
#define ADD(Key, Name)                          \
    ksets.insert(pair(#Key, Name))
    ADD(b,   "char_b_law");
    ADD(br,  "char_ratexpset<char_b_law>_law");
    ADD(z,   "char_z_law");
    ADD(zr,  "char_ratexpset<char_z_law>_law");
    ADD(zrr, "char_ratexpset<char_ratexpset<char_z_law>_law>_law");
#undef ADD
  while ((opt = getopt(argc, argv, "aeg:hi:o:L:W:?")) != -1)
    switch (opt)
      {
      case 'a':
        opts.is_automaton = true;
        opts.input_format = vcsn::dyn::FileType::dotty;
        opts.output_format = vcsn::dyn::FileType::dotty;
        break;
      case 'e':
        opts.is_automaton = false;
        opts.input_format = vcsn::dyn::FileType::text;
        opts.output_format = vcsn::dyn::FileType::text;
        break;
      case 'g':
        opts.labelset_describ = optarg;
        break;
      case 'h':
        usage(argv[0], EXIT_SUCCESS);
        break;
      case 'i':
        opts.input_format = string_to_file_type(optarg);
        break;
      case 'o':
        opts.output_format = string_to_file_type(optarg);
        break;
      case 'L':
        {
          std::string s = optarg;
          if (s == "l" || s == "letter" || s == "letters")
            opts.lal = true;
          else if (s == "w" || s == "word" || s == "words")
            opts.lal = false;
          else
            {
              std::cerr << optarg << ": invalid label kind (-L)" << std::endl;
              goto fail;
            }
          break;
        }
      case 'W':
        {
          map::iterator i = ksets.find(optarg);
          if (i == end(ksets))
            opts.context = optarg;
          else
            opts.context = i->second;
          break;
        }
      case '?':
      fail:
        usage(argv[0], EXIT_FAILURE);
        break;
      }
  argc -= optind;
  argv += optind;
  if (argc < 1)
    {
      std::cerr << "invalid number of arguments: " << argc << std::endl;
      usage(prog, EXIT_FAILURE);
    }
  opts.file = argv[0];
  return opts;
}
