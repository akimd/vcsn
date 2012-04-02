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



void
pp(const std::shared_ptr<vcsn::factory> factory, const char* s, bool file)
{
  vcsn::rat::driver d(*factory);
  if (vcsn::rat::exp* e = file ? d.parse_file(s) : d.parse_string(s))
    {
      factory->print(std::cout, e) << std::endl;
      delete e;
    }
  else
    {
      std::cerr << d.errors << std::endl;
      exit(EXIT_FAILURE);
    }
}

#define COMMA ,

int
main(int argc, char* const argv[])
{
  int opt;
  std::map<std::string, std::shared_ptr<vcsn::factory>> factories;
  typedef vcsn::set_alphabet<vcsn::char_letters> alpha_t;
  alpha_t alpha{'a', 'b', 'c', 'd'};
#define DEFINE(Name, Param, Arg)                                        \
  factories                                                             \
    .insert(std::make_pair                                              \
            (std::string(#Name),                                        \
             std::shared_ptr<vcsn::factory_<alpha_t, Param>>            \
             (new vcsn::factory_<alpha_t, Param>{ alpha, Arg })))
  DEFINE(b, vcsn::b, vcsn::b());
  DEFINE(br, vcsn::factory_<alpha_t COMMA vcsn::b>, factories["b"].get());
  DEFINE(z, vcsn::z, vcsn::z());
  DEFINE(zr, vcsn::factory_<alpha_t COMMA vcsn::z>, factories["z"].get());
  DEFINE(zrr, vcsn::factory_<alpha_t COMMA vcsn::factory_<alpha_t COMMA vcsn::z>>, factories["zr"].get());
#undef DEFINE
  std::string w = "b";
  while ((opt = getopt(argc, argv, "e:f:hw:")) != -1)
    switch (opt)
      {
      case 'e':
        pp(factories[w], optarg, false);
        break;
      case 'f':
        pp(factories[w], optarg, true);
        break;
      case 'w':
        w = optarg;
        if (factories.find(w) == end(factories))
          {
            std::cerr << w << ": invalid weight set" << std::endl;
            usage(argv[0], EXIT_FAILURE);
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
    pp(factories[w], argv[i], false);
}
