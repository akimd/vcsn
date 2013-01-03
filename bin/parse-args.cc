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
      << "usage: " << prog << " [OPTIONS...] exp\n"
      "\n"
      "Context:\n"
      "  -a automaton           An automaton file.\n"
      "  -e expression          An expression.\n"
      "  -L letter|words        Kind of the labels.\n"
      "  -W WEIGHT-SET          Define the kind of the weights.\n"
      "  -g string              Generator set definition.\n"
      "Format:\n"
      "  -i FORMAT              Input format.\n"
      "  -o FORMAT              Output format.\n"
      "WEIGHT-SET:\n"
      "  b        for Boolean\n"
      "  br       for RatExp<b>\n"
      "  z        for Z\n"
      "  zr       for RatExp<Z>\n"
      "  zrr      for Ratexp<RatExp<Z>>\n"
      ;
  else
    std::cerr << "Try `" << prog << "-h' for more information."
              << std::endl;
  exit(exit_status);
}

options
parse_args(int* argc, char* const * argv[])
{
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
  while ((opt = getopt(*argc, *argv, "g:hi:o:L:W:?")) != -1)
    switch (opt)
      {
      case 'a':
        opts.is_automaton = true;
        opts.file = optarg;
        break;
      case 'e':
        opts.is_automaton = false;
        opts.file = optarg;
      case 'g':
        opts.labelset_describ = optarg;
        break;
      case 'h':
        usage(argv[0][0], EXIT_SUCCESS);
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
            {
              std::cerr << optarg << ": invalid weight set (-W)" << std::endl;
              goto fail;
            }
          else
            opts.context = i->second;
          break;
        }
      case '?':
      fail:
        usage(argv[0][0], EXIT_FAILURE);
        break;
      }
  *argc -= optind;
  *argv += optind;
  return opts;
}
