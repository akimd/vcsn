#include <cstdlib>
#include <memory>
#include <map>
#include <getopt.h>

#include <vcsn/weights/b.hh>
#include <vcsn/weights/z.hh>
#include <vcsn/core/rat/factory_.hh>
#include <vcsn/io/driver.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>

#include <vcsn/algos/dotty.hh>
#include <vcsn/core/mutable_automaton.hh>

static
void
usage(const char* prog, int status)
{
  if (status == EXIT_SUCCESS)
    std::cout <<
      "usage: " << prog << " [OPTION...] [EXP...]\n"
      "\n"
      "Options:\n"
      "  -w WEIGHT-SET   define the kind of the weights [b]\n"
      "  -e EXP          pretty-print the rational expression EXP\n"
      "  -f FILE         pretty-print the rational expression in FILE\n"
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

// FIXME: No globals.

enum type
  {
    b, br,
    z, zr, zrr,
  };

using alpha_t
  = vcsn::set_alphabet<vcsn::char_letters>;
alpha_t alpha{'a', 'b', 'c', 'd'};

#define COMMA ,

template <type Type>
struct factory{};

#define DEFINE(Name, Param, Arg)                        \
  auto fact_ ## Name                                    \
  = vcsn::factory_<alpha_t, Param>{ alpha, Arg };       \
                                                        \
  template <>                                           \
  struct factory<Name>                                  \
  {                                                     \
    const vcsn::factory_<alpha_t, Param>&               \
      get()                                             \
    {                                                   \
      return fact_ ## Name;                             \
    }                                                   \
  };

  DEFINE(b, vcsn::b, vcsn::b());
  DEFINE(br, vcsn::factory_<alpha_t COMMA vcsn::b>, fact_b);
  DEFINE(z, vcsn::z, vcsn::z());
  DEFINE(zr, vcsn::factory_<alpha_t COMMA vcsn::z>, fact_z);
  DEFINE(zrr, vcsn::factory_<alpha_t COMMA vcsn::factory_<alpha_t COMMA vcsn::z>>, fact_zr);
#undef DEFINE

template<typename Factory>
void
pp(const Factory& factory, const char* s, bool file)
{
  using weightset_t
    = typename Factory::weightset_t;
  using automaton_t
    = vcsn::mutable_automaton<alpha_t, weightset_t, vcsn::labels_are_words>;
  vcsn::rat::driver d(factory);
  if (vcsn::rat::exp* e = file ? d.parse_file(s) : d.parse_string(s))
    {
      factory.print(std::cout, e) << std::endl;
      delete e;
    }
  else
    {
      std::cerr << d.errors << std::endl;
      exit(EXIT_FAILURE);
    }
}

void
pp(const type& ws, const char* s, bool file)
{
  switch (ws)
    {
#define CASE(Name)                              \
      case Name:                                \
        pp(fact_ ## Name, s, file);             \
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
{
  using map = std::map<std::string, type>;
  using pair = std::pair<std::string, type>;

  map factories;
#define DEFINE(Name)                            \
  factories.insert(pair(#Name, Name))
  DEFINE(b);
  DEFINE(br);
  DEFINE(z);
  DEFINE(zr);
  DEFINE(zrr);
#undef DEFINE

  type w = b;
  int opt;
  while ((opt = getopt(argc, argv, "e:f:hw:")) != -1)
    switch (opt)
      {
      case 'e':
        pp(w, optarg, false);
        break;
      case 'f':
        pp(w, optarg, true);
        break;
      case 'w':
        {
          map::iterator i = factories.find(optarg);
          if (i == end(factories))
            {
              std::cerr << optarg << ": invalid weight set" << std::endl;
              usage(argv[0], EXIT_FAILURE);
            }
          else
            w = i->second;
        }
        break;
      case 'h':
        usage(argv[0], EXIT_SUCCESS);
        break;
      case '?':
        usage(argv[0], EXIT_FAILURE);
        break;
      }
  argc -= optind;
  argv += optind;
  for (int i = 0; i < argc; ++i)
    pp(w, argv[i], false);
}
