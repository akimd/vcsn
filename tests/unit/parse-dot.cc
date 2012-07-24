#include <lib/vcsn/dot/driver.hh>
#include <vcsn/ctx/char_b_lal.hh>

int
main (int argc, char* const argv[])
{
  vcsn::ctx::char_b_lal ctx{{'a', 'b', 'c'}};
  vcsn::concrete_abstract_kratexpset<vcsn::ctx::char_b_lal> fac{ctx};
  vcsn::dot::driver d{fac};
  for (int i = 1; i < argc; ++i)
    {
      d.parse_file(argv[i]);
      std::cerr << d.errors << std::endl;
    }
}
