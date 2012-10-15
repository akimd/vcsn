#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dotty.hh>
#include <lib/vcsn/dot/driver.hh>

int
main (int argc, char* const argv[])
{
  vcsn::dot::driver d;
  for (int i = 1; i < argc; ++i)
    {
      auto *aut = d.parse_file(argv[i]);
      std::cerr << d.errors << std::endl;
      if (aut)
        {
          auto *res = vcsn::determinize(*aut);
          dotty(*res, std::cout);
        }
      else
        return 1;
    }
}
