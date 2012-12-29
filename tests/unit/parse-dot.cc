#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/dot/driver.hh>

int
main (int argc, char* const argv[])
{
  vcsn::dot::driver d;
  for (int i = 1; i < argc; ++i)
    {
      auto aut = d.parse_file(argv[i]);
      std::cerr << d.errors << std::endl;
      if (aut)
        // FIXME: To remove when in dyn::
        std::cout << vcsn::dyn::dotty(aut) << '\n';
      else
        return 1;
    }
}
