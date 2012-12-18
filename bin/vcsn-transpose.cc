#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include <vcsn/dyn/ratexp.hh>

#include <lib/vcsn/rat/driver.hh>

int main(const int argc, char *const argv[])
{
  assert(argc == 4);
  std::string ctx_str = argv[1];
  std::string genset_str = argv[2];
  std::string ratexp_str = argv[3];

  // Input.
  using namespace vcsn::dyn;
  context* ctx  = make_context(ctx_str, genset_str);
  vcsn::rat::driver d(*ctx);
  // FIXME: be sure to keep ratexp alive!
  auto ratexp = d.parse_string(ratexp_str);
  vcsn::dyn::ratexp exp = make_ratexp(*ctx, ratexp);

  // Process.
  vcsn::dyn::ratexp res = transpose (exp);

  // Output.
  print(res, std::cout) << std::endl;
}
