#include <cstdlib>
#include <memory>
#include <map>
#include <getopt.h>

#include <vcsn/algos/dyn.hh>
#include <vcsn/algos/dotty.hh>
#include <vcsn/algos/lift.hh>
#include <vcsn/algos/make-context.hh>
#include <vcsn/algos/standard_of.hh>
#include <vcsn/algos/transpose.hh>

#include <lib/vcsn/rat/driver.hh>

static
void
usage(const char* prog, int status)
{
  if (status == EXIT_SUCCESS)
    std::cout <<
      "usage: " << prog << " [OPTION...] [EXP...]\n"
      "\n"
      "Context:\n"
      "  -H HEURISTICS     define the aut-to-exp heuristics to use [order]\n"
      "  -L letters|words  kind of the labels [letters]\n"
      "  -W WEIGHT-SET     define the kind of the weights [b]\n"
      "  -e EXP            pretty-print the rational expression EXP\n"
      "  -f FILE           pretty-print the rational expression in FILE\n"
      "\n"
      "Operations:\n"
      "  -t    transpose the rational expression (accumulates)\n"
      "  -s    display the standard automaton instead of expression\n"
      "  -l    display the lifted standard automaton instead of expression\n"
      "  -T    transpose the automaton\n"
      "  -a    display the aut-to-exp of the standard automaton\n"
      "\n"
      "WEIGHT-SET:\n"
      "  b    for Boolean\n"
      "  br   for RatExp<b>\n"
      "  z    for Z\n"
      "  zr   for RatExt<Z>\n"
      "  zrr  for RatExp<RatExp<Z>>\n"
      ;
  else
    std::cerr << "Try `" << prog << " -h' for more information."
              << std::endl;
  exit(status);
}

enum class type
{
  b, br,
  z, zr, zrr,
};

enum class heuristics
{
  order,
  degree,
};

struct options
{
  bool labels_are_letters = true;
  std::string context = "char_b_lal";
  size_t transpose = 0;
  bool aut_transpose = false;
  bool standard_of = false;
  bool lift = false;
  bool aut_to_exp = false;
  heuristics next = heuristics::order;
};

void
transpose(const vcsn::ctx::abstract_context& ctx, const vcsn::rat::exp_t e)
{
  auto *aut1 = vcsn::standard_of(ctx, e);
  if (!!getenv("DEBUG"))
    {
      std::cerr << aut1->vname() << std::endl;
      vcsn::dotty(*aut1, std::cout);
    }
  auto *aut2 = vcsn::transpose(*aut1);
  if (!!getenv("DEBUG"))
    std::cerr << aut2->vname() << std::endl;
  vcsn::dotty(*aut2, std::cout);
  delete aut2;
  delete aut1;
}

void
abstract_pp(const options& opts, const vcsn::ctx::abstract_context& ctx,
            const char* s, bool file)
{
  vcsn::rat::driver d(ctx);
  auto* kset = vcsn::make_kratexpset(ctx);
  if (auto exp = file ? d.parse_file(s) : d.parse_string(s))
    {
      for (size_t i = 0; i < opts.transpose; ++i)
        exp = vcsn::transpose(ctx, exp);

      if (opts.aut_transpose)
        transpose(ctx, exp);
      else if (opts.standard_of || opts.lift || opts.aut_to_exp)
        {
          auto aut = vcsn::standard_of(ctx, exp);
          if (opts.standard_of)
            vcsn::dotty(*aut, std::cout);
          if (opts.lift)
            vcsn::dotty(*vcsn::lift(*aut), std::cout);
          if (opts.aut_to_exp)
            switch (opts.next)
            {
            case heuristics::degree:
              std::cout << kset->format(vcsn::dyn::aut_to_exp_in_degree(*aut)) << std::endl;
              break;
            case heuristics::order:
              std::cout << kset->format(vcsn::dyn::aut_to_exp(*aut)) << std::endl;
              break;
            }
        }
      else
        kset->print(std::cout, exp) << std::endl;
    }
  else
    {
      std::cerr << d.errors << std::endl;
      exit(EXIT_FAILURE);
    }
}

void
pp(const options& opts, const char* s, bool file)
{
  std::string ctx = opts.context;
  if (opts.labels_are_letters)
    for (char& c: ctx)
      if (c == 'w')
        c = 'l';
  if (!!getenv("YYDEBUG"))
    std::cerr << "Loading: " << ctx << std::endl;
  abstract_pp(opts, *vcsn::make_context(ctx, "abcd"), s, file);
}

int
main(int argc, char* const argv[])
try
{
  using map = std::map<std::string, std::string>;
  using pair = std::pair<std::string, std::string>;

  map ksets;
#define DEFINE(Key, Name)                         \
  ksets.insert(pair(#Key, Name))
  DEFINE(b,   "char_b_law");
  DEFINE(br,  "char_kratexpset<char_b_law>_law");
  DEFINE(z,   "char_z_law");
  DEFINE(zr,  "char_kratexpset<char_z_law>_law");
  DEFINE(zrr, "char_kratexpset<char_kratexpset<char_z_law>_law>_law");
#undef DEFINE

  options opts;
  int opt;
  while ((opt = getopt(argc, argv, "ae:f:H:hL:lsTtW:")) != -1)
    switch (opt)
      {
      case 'a':
        opts.aut_to_exp = true;
        break;
      case 'e':
        pp(opts, optarg, false);
        break;
      case 'f':
        pp(opts, optarg, true);
        break;
      case 'H':
        {
          std::string s = optarg;
          if (s == "d" || s == "degree")
            opts.next = heuristics::degree;
          else if (s == "o" || s == "order")
            opts.next = heuristics::order;
          else
            {
              std::cerr << optarg << ": invalid heuristics (-H)" << std::endl;
              goto fail;
            }
          break;
        }
      case 'h':
        usage(argv[0], EXIT_SUCCESS);
        break;
      case 'L':
        {
          std::string s = optarg;
          if (s == "l" || s == "letter" || s == "letters")
            opts.labels_are_letters = true;
          else if (s == "w" || s == "word" || s == "words")
            opts.labels_are_letters = false;
          else
            {
              std::cerr << optarg << ": invalid label kind (-L)" << std::endl;
              goto fail;
            }
          break;
        }
        break;
      case 'l':
        opts.lift = true;
        break;
      case 's':
        opts.standard_of = true;
        break;
      case 'T':
        ++opts.aut_transpose;
        break;
      case 't':
        ++opts.transpose;
        break;
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
        }
        break;
      case '?':
      fail:
        usage(argv[0], EXIT_FAILURE);
        break;
      }
  argc -= optind;
  argv += optind;
  for (int i = 0; i < argc; ++i)
    pp(opts, argv[i], false);
}
catch (const std::exception& e)
{
  std::cerr << argv[0] << ": " << e.what() << std::endl;
  exit(EXIT_FAILURE);
}
