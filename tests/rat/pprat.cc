#include <cstdlib>
#include <memory>
#include <map>
#include <getopt.h>

#include <vcsn/algos/dotty.hh>
#include <vcsn/algos/lift.hh>
#include <vcsn/algos/standard_of.hh>
#include <vcsn/algos/aut_to_exp.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/core/rat/abstract_kratexpset.hh>
#include <vcsn/core/rat/kratexpset.hh>
#include <vcsn/ctx/char.hh>
#include <vcsn/io/driver.hh>

static
void
usage(const char* prog, int status)
{
  if (status == EXIT_SUCCESS)
    std::cout <<
      "usage: " << prog << " [OPTION...] [EXP...]\n"
      "\n"
      "Context:\n"
      "  -A letters|words  kind of the atoms [letters]\n"
      "  -H HEURISTICS     define the aut-to-exp heuristics to use [order]\n"
      "  -W WEIGHT-SET     define the kind of the weights [b]\n"
      "  -e EXP            pretty-print the rational expression EXP\n"
      "  -f FILE           pretty-print the rational expression in FILE\n"
      "\n"
      "Operations:\n"
      "  -l    display the lifted standard automaton instead of expression\n"
      "  -s    display the standard automaton instead of expression\n"
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
  bool atoms_are_letters = true;
  type weight = type::b;
  bool standard_of = false;
  bool lift = false;
  bool aut_to_exp = false;
  heuristics next = heuristics::order;
};

// FIXME: No globals.

using lal = vcsn::labels_are_letters;
using law = vcsn::labels_are_words;
using b = vcsn::b;
using z = vcsn::z;

template <typename T, typename Kind>
using ks = vcsn::kratexpset<vcsn::ctx::char_<T, Kind>>;
template <typename T>
using ksl = ks<T, lal>;
template <typename T>
using ksw = ks<T, law>;

#define DEFINE(Name, Kind, Param, Arg)                          \
  auto ctx_ ## Name =                                           \
    vcsn::ctx::char_<Param, Kind> {{'a', 'b', 'c', 'd'}, Arg }; \
  auto ks_ ## Name = ks<Param, Kind>{ ctx_ ## Name };

  DEFINE(b,   lal, b,           b());
  DEFINE(br,  lal, ksl<b>,      ks_b);
  DEFINE(z,   lal, z,           z());
  DEFINE(zr,  lal, ksl<z>,      ks_z);
  DEFINE(zrr, lal, ksl<ksl<z>>, ks_zr);

  DEFINE(bw,   law, b,           b());
  DEFINE(brw,  law, ksw<b>,      ks_bw);
  DEFINE(zw,   law, z,           z());
  DEFINE(zrw,  law, ksw<z>,      ks_zw);
  DEFINE(zrrw, law, ksw<ksw<z>>, ks_zrw);
#undef DEFINE

template<typename KSet>
void
pp(const options& opts, const KSet& kset,
   const char* s, bool file)
{
  using context_t = typename KSet::context_t;
  vcsn::concrete_abstract_kratexpset<context_t> fac{kset.context()};
  vcsn::rat::driver d(fac);
  if (auto exp = file ? d.parse_file(s) : d.parse_string(s))
    {
      auto e = kset.context().downcast(exp);
      if (opts.standard_of || opts.lift || opts.aut_to_exp)
        {
          using automaton_t = vcsn::mutable_automaton<context_t>;
          auto aut = vcsn::standard_of<automaton_t>(kset.context(), e);
          if (opts.standard_of)
            vcsn::dotty(aut, std::cout);
          if (opts.lift)
            vcsn::dotty(vcsn::lift(aut), std::cout);
          if (opts.aut_to_exp)
            switch (opts.next)
            {
            case heuristics::degree:
              std::cout << kset.format(vcsn::aut_to_exp_in_degree(aut)) << std::endl;
              break;
            case heuristics::order:
              std::cout << kset.format(vcsn::aut_to_exp(aut)) << std::endl;
              break;
            }
        }
      else
        kset.print(std::cout, e) << std::endl;
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
  switch (opts.weight)
    {
#define CASE(Name)                                      \
      case type::Name:                                  \
        if (opts.atoms_are_letters)                     \
          pp(opts, ks_ ## Name, s, file);               \
        else                                            \
          pp(opts, ks_ ## Name ## w, s, file);          \
      break;
      CASE(b);
      CASE(br);
      CASE(z);
      CASE(zr);
      CASE(zrr);
#undef CASE
    }
}

int
main(int argc, char* const argv[])
try
{
  using map = std::map<std::string, type>;
  using pair = std::pair<std::string, type>;

  map ksets;
#define DEFINE(Name)                            \
  ksets.insert(pair(#Name, type::Name))
  DEFINE(b);
  DEFINE(br);
  DEFINE(z);
  DEFINE(zr);
  DEFINE(zrr);
#undef DEFINE

  options opts;
  int opt;
  while ((opt = getopt(argc, argv, "A:ae:f:H:hlsW:")) != -1)
    switch (opt)
      {
      case 'A':
        {
          std::string s = optarg;
          if (s == "l" || s == "letter" || s == "letters")
            opts.atoms_are_letters = true;
          else if (s == "w" || s == "word" || s == "words")
            opts.atoms_are_letters = false;
          else
            {
              std::cerr << optarg << ": invalid atom kind (-A)" << std::endl;
              goto fail;
            }
          break;
        }
        break;
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
      case 'l':
        opts.lift = true;
        break;
      case 's':
        opts.standard_of = true;
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
            opts.weight = i->second;
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
