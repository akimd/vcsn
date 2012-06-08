#include <cstdlib>
#include <memory>
#include <map>
#include <getopt.h>

#include <vcsn/algos/dotty.hh>
#include <vcsn/algos/standard_of.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/core/rat/kratexps.hh>
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
      "Options:\n"
      "  -a letters|words  kind of the atoms [letters]\n"
      "  -w WEIGHT-SET     define the kind of the weights [b]\n"
      "  -e EXP            pretty-print the rational expression EXP\n"
      "  -f FILE           pretty-print the rational expression in FILE\n"
      "  -s                display the standard automaton instead of expression\n"
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


struct options
{
  bool atoms_are_letters;
  type weight;
  bool standard_of;
};

// FIXME: No globals.

using lal = vcsn::labels_are_letters;
using law = vcsn::labels_are_words;
using b = vcsn::b;
using z = vcsn::z;

template <typename T, typename Kind>
using kre = vcsn::kratexps<vcsn::ctx::char_<T>, Kind>;
template <typename T>
using krel = kre<T, lal>;
template <typename T>
using krew = kre<T, law>;

#define DEFINE(Name, Kind, Param, Arg)                          \
  auto ctx_ ## Name =                                           \
    vcsn::ctx::char_<Param> {{'a', 'b', 'c', 'd'}, Arg };       \
  auto fact_ ## Name =                                          \
    kre<Param, Kind>{ ctx_ ## Name };

  DEFINE(b,   lal, b,             b());
  DEFINE(br,  lal, krel<b>,       fact_b);
  DEFINE(z,   lal, z,             z());
  DEFINE(zr,  lal, krel<z>,       fact_z);
  DEFINE(zrr, lal, krel<krel<z>>, fact_zr);

  DEFINE(bw,   law, b,             b());
  DEFINE(brw,  law, krew<b>,       fact_bw);
  DEFINE(zw,   law, z,             z());
  DEFINE(zrw,  law, krew<z>,       fact_zw);
  DEFINE(zrrw, law, krew<krew<z>>, fact_zrw);
#undef DEFINE

template<typename Factory>
void
pp(const options& opts, const Factory& factory,
   const char* s, bool file)
{
  vcsn::rat::driver d(factory);
  if (auto e = file ? d.parse_file(s) : d.parse_string(s))
    {
      if (opts.standard_of)
        {
          using context_t = typename Factory::context_t;
          using automaton_t = vcsn::mutable_automaton<context_t>;
          auto aut = vcsn::rat::standard_of<automaton_t>(factory.context(), e);
          vcsn::dotty(aut, std::cout);
        }
      else
        factory.print(std::cout, e) << std::endl;
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
          pp(opts, fact_ ## Name, s, file);             \
        else                                            \
          pp(opts, fact_ ## Name ## w, s, file);        \
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

  map factories;
#define DEFINE(Name)                            \
  factories.insert(pair(#Name, type::Name))
  DEFINE(b);
  DEFINE(br);
  DEFINE(z);
  DEFINE(zr);
  DEFINE(zrr);
#undef DEFINE

  options opts =
    {
      .atoms_are_letters = true,
      .weight = type::b,
      .standard_of = false,
    };
  int opt;
  while ((opt = getopt(argc, argv, "a:e:f:hsw:")) != -1)
    switch (opt)
      {
      case 'a':
        {
          std::string s = optarg;
          if (s == "l" || s == "letter" || s == "letters")
            opts.atoms_are_letters = true;
          else if (s == "w" || s == "word" || s == "words")
            opts.atoms_are_letters = false;
          else
            {
              std::cerr << optarg << ": invalid atom kind (-a)" << std::endl;
              goto fail;
            }
          break;
        }
        break;
      case 'e':
        pp(opts, optarg, false);
        break;
      case 'f':
        pp(opts, optarg, true);
        break;
      case 'h':
        usage(argv[0], EXIT_SUCCESS);
        break;
      case 's':
        opts.standard_of = true;
        break;
      case 'w':
        {
          map::iterator i = factories.find(optarg);
          if (i == end(factories))
            {
              std::cerr << optarg << ": invalid weight set (-w)" << std::endl;
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
