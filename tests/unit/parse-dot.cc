#include <lib/vcsn/dot/driver.hh>

int
main (int argc, char* const argv[])
{
  vcsn::dot::driver d;
  for (int i = 1; i < argc; ++i)
    {
      d.parse_file(argv[i]);
      std::cerr << d.errors << std::endl;
    }
}
