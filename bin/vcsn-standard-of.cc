#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>

int main(const int argc, char *const argv[])
{
  assert(argc == 4);
  std::string ctx_str = argv[1];
  std::string labelset_str = argv[2];
  std::string ratexp_file = argv[3];

  // Input.
  using namespace vcsn::dyn;
  vcsn::dyn::context* ctx = make_context(ctx_str, labelset_str);
  vcsn::dyn::ratexp exp = read_ratexp_file(ratexp_file, *ctx);

  // Process.
  vcsn::dyn::automaton aut = standard_of(exp);

  // Output.
  print(aut, std::cout) << std::endl;
}
