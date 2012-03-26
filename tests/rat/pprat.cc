#include <cstdlib>

#include <vcsn/weights/b.hh>
#include <vcsn/weights/z.hh>
#include <vcsn/core/rat/factory_.hh>
#include <vcsn/io/driver.hh>

static
void
usage(const char* prog, int status)
{
  std::cerr <<
    "usage: " << prog << " WEIGHT-SET\n"
    "\n"
    "WEIGHT-SET:\n"
    "  z    for Z\n"
    "  b    for Boolean\n"
    "  rat  for RatExp<b>\n";
  exit(status);
}

template <typename WeightSet>
int
pp(const WeightSet& ws)
{
  typedef WeightSet weightset_t;
  typedef typename weightset_t::value_t weight_t;
  vcsn::factory_<weightset_t> factory(ws);
  vcsn::rat::driver d(factory);
  if (vcsn::rat::exp* e = d.parse_file("-"))
    factory.print(std::cout, e) << std::endl;
  return 1;
}

int
main(int argc, const char* argv[])
{
  if (argc != 2)
    usage(argv[0], EXIT_FAILURE);
  std::string w(argv[1]);

  if (w == "z")
    return pp(vcsn::z());
  else if (w == "b")
    return pp(vcsn::b());
  else if (w == "rat")
    {
      vcsn::factory_<vcsn::b> rat{ vcsn::b() };
      return pp(rat);
    }
  else
    usage(argv[0], EXIT_FAILURE);
}
