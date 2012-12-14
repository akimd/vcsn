#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include <vcsn/dyn/ratexp.hh>

#include <lib/vcsn/rat/driver.hh>

int main(const int argc, char *const argv[])
{
  assert(argc == 3);
  using namespace vcsn::dyn;

  std::string cname = argv[1];
  context* ctx  = make_context(cname, "ab");
  vcsn::rat::driver d(*ctx);

  auto exp = d.parse_string(argv[2]);

  auto dyn_exp = make_ratexp(*ctx, exp);

  dotty(standard_of(dyn_exp), std::cout);
}
