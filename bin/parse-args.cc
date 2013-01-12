#include <getopt.h>

#include <iostream>
#include <map>
#include <stdexcept>

// Temporary.
#include <boost/algorithm/string/replace.hpp>

#include "parse-args.hh"
#include <vcsn/algos/dyn.hh>

vcsn::dyn::FileType
string_to_file_type(const std::string str)
{
  using vcsn::dyn::FileType;
  if (str == "dot")
    return FileType::dot;
  else if (str == "text")
    return FileType::text;
  else if (str == "xml")
    return FileType::xml;
  else
    throw std::domain_error(str + " is an invalid file type.");
}

vcsn::dyn::automaton
read_automaton(const options& opts)
{
  return
    opts.input_is_file
    ? vcsn::dyn::read_automaton_file(opts.input)
    : vcsn::dyn::read_automaton_string(opts.input);
}

vcsn::dyn::ratexp
read_ratexp(const options& opts)
{
  auto ctx = vcsn::dyn::make_context(opts.context, opts.labelset_describ);
  return
    opts.input_is_file
    ? vcsn::dyn::read_ratexp_file(opts.input, *ctx, opts.input_format)
    : vcsn::dyn::read_ratexp_string(opts.input, *ctx, opts.input_format);
}

void
usage(const char* prog, int exit_status)
{
  if (exit_status == EXIT_SUCCESS)
    std::cout
      << "usage: " << prog << " [OPTIONS...]\n"
      "\n"
      "Context:\n"
      "  -A                 input is an automaton\n"
      "  -E                 input is a rational expression\n"
      "  -e STRING          the input is STRING\n"
      "  -f FILE            the input is FILE\n"
      "  -C CONTEXT         the context to use\n"
      "  -L letter|words    kind of the labels\n"
      "  -W WEIGHT-SET      define the kind of the weights\n"
      "  -g STRING          generator set definition\n"
      "\n"
      "Format:\n"
      "  -i FORMAT          input format\n"
      "  -o FORMAT          output format\n"
      "\n"
      "Context:\n"
      "  la(l|u|w)_char_(b|z|zmin)\n"
      "  etc.\n"
      "\n"
      "WeightSet:\n"
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

// Convert "w" to "l" for "brutal" assembled context names.
void
apply_label_kind(options& opts)
{
  using boost::algorithm::replace_all;
  if (opts.lal)
    replace_all(opts.context, "law", "lal");
  else
    replace_all(opts.context, "lal", "law");
}

void
parse_args(options& opts, int& argc, char* const*& argv)
{
  using map = std::map<std::string, std::string>;
  using pair = std::pair<std::string, std::string>;

  int opt;
  map ksets;
#define ADD(Key, Name)                          \
    ksets.insert(pair(#Key, Name))
    ADD(b,   "law_char_b");
    ADD(br,  "law_char_ratexpset<law_char_b>");
    ADD(z,   "law_char_z");
    ADD(zr,  "law_char_ratexpset<law_char_z>");
    ADD(zrr, "law_char_ratexpset<law_char_ratexpset<law_char_z>>");
#undef ADD
  while ((opt = getopt(argc, argv, "AC:Ee:f:g:hi:o:L:W:?")) != -1)
    switch (opt)
      {
      case 'A':
        opts.is_automaton = true;
        opts.input_format = vcsn::dyn::FileType::dot;
        break;
      case 'C':
        opts.context = optarg;
        break;
      case 'E':
        opts.is_automaton = false;
        opts.input_format = vcsn::dyn::FileType::text;
        break;
      case 'e':
        opts.input = optarg;
        opts.input_is_file = false;
        break;
      case 'f':
        opts.input = optarg;
        opts.input_is_file = true;
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
          apply_label_kind(opts);
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
            {
              opts.context = i->second;
              apply_label_kind(opts);
            }
          break;
        }
      case '?':
      fail:
        usage(argv[0], EXIT_FAILURE);
        break;
      }
  argc -= optind;
  argv += optind;
}

options
parse_args(int& argc, char* const*& argv)
{
  options res;
  parse_args(res, argc, argv);
  return res;
}
