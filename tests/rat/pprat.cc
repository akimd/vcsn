#include <cstdlib>
#include <getopt.h>

#include <vcsn/weights/b.hh>
#include <vcsn/weights/z.hh>
#include <vcsn/core/rat/factory_.hh>
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
      "  -w WEIGHT-SET   define the kind of the weights [b]\n"
      "  -e EXP          pretty-print the rational expression EXP\n"
      "  -f FILE         pretty-print the rational expression in FILE\n"
      "\n"
      "WEIGHT-SET:\n"
      "  z    for Z\n"
      "  b    for Boolean\n"
      "  rat  for RatExp<b>\n";
  else
    std::cerr << "Try `" << prog << " --help' for more information."
              << std::endl;
  exit(status);
}

const vcsn::factory_<vcsn::b> b_fact{ vcsn::b() };
const vcsn::factory_<vcsn::z> z_fact{ vcsn::z() };
const vcsn::factory_<vcsn::factory_<vcsn::b>> rat_fact{ b_fact };


void
pp(const std::string& w, const char* s, bool file)
{
  const vcsn::factory* factory = nullptr;
  if (w == "b")
    factory = &b_fact;
  else if (w == "rat")
    factory = &rat_fact;
  else
    factory = &z_fact;
  vcsn::rat::driver d(*factory);
  if (vcsn::rat::exp* e = file ? d.parse_file(s) : d.parse_string(s))
    factory->print(std::cout, e) << std::endl;
  else
    exit(EXIT_FAILURE);
}

int
main(int argc, char* const argv[])
{
  int opt;
  std::string w = "b";
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
        w = optarg;
        if (w != "b" && w != "rat" && w != "z")
          usage(argv[0], EXIT_FAILURE);
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
