#include <cstdlib>
#include <memory>
#include <map>
#include <getopt.h>

#include <vcsn/algos/lift.hh>
#include <vcsn/algos/aut_to_exp.hh>
#include <vcsn/algos/transpose.hh>
#include <vcsn/core/kind.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/core/rat/abstract_kratexpset.hh>
#include <vcsn/core/rat/kratexpset.hh>
#include <vcsn/ctx/char_b_lal.hh>
#include <vcsn/ctx/char_b_law.hh>
#include <vcsn/ctx/char_z_lal.hh>
#include <vcsn/ctx/char_z_law.hh>

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
  type weight = type::b;
  size_t transpose = 0;
  bool aut_transpose = false;
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

template<typename Context>
void
pp(const options& opts, const Context& ctx,
   const char* s, bool file)
{
  using context_t = Context;
  vcsn::concrete_abstract_kratexpset<context_t> fac{ctx};
  using kset_t = vcsn::kratexpset<context_t>;
  kset_t kset{ctx};

  vcsn::rat::driver d(fac);
  if (auto exp = file ? d.parse_file(s) : d.parse_string(s))
    {
      auto e = ctx.downcast(exp);
      for (size_t i = 0; i < opts.transpose; ++i)
        e = kset.transpose(e);

      if (opts.aut_transpose)
        {
          using automaton_t = vcsn::mutable_automaton<context_t>;
          auto aut1 = vcsn::standard_of<automaton_t>(ctx, e);
          auto aut2 = vcsn::transpose(aut1);
          vcsn::dotty(aut2, std::cout);
        }
      else if (opts.standard_of || opts.lift || opts.aut_to_exp)
        {
          using automaton_t = vcsn::mutable_automaton<context_t>;
          auto aut = vcsn::standard_of<automaton_t>(ctx, e);
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
#define CASE(Name)                              \
      case type::Name:                          \
        if (opts.labels_are_letters)            \
          pp(opts, ctx_ ## Name, s, file);      \
        else                                    \
          pp(opts, ctx_ ## Name ## w, s, file); \
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
