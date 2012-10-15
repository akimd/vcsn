#include <vcsn/core/mutable_automaton.hh>
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
        {
          // FIXME: Remove vcsn:: when in dyn::
          auto res = vcsn::dyn::determinize(aut);
          vcsn::dyn::dotty(res, std::cout);
        }
      else
        return 1;
    }
}
