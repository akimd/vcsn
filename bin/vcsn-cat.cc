#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh> // make_context
#include <vcsn/dyn/ratexp.hh>
// FIXME: REMOVE THIS AND USE DYN::OUTPUT.
#include <vcsn/core/rat/abstract_ratexpset.hh> // kset->format

#include <lib/vcsn/rat/driver.hh>

int main(const int argc, char *const argv[])
{
  assert(argc == 4);
  std::string ctx_str = argv[1];
  std::string genset_str = argv[2];
  std::string ratexp_str = argv[3];

  // Input.
  // FIXME: SHOULD BE A ONE-LINER.
  using namespace vcsn::dyn;
  vcsn::dyn::context* ctx = make_context(ctx_str, genset_str);
  vcsn::rat::driver d(*ctx);
  // FIXME: We cannot write
  //
  // vcsn::dyn::ratexp exp = make_ratexp(*ctx, d.parse_string(argv[arg_ratexp]));
  // because d.parse_string does not allocate memory.  This is
  // extremely misleading and dangerous.
  auto ratexp = d.parse_string(ratexp_str);
  vcsn::dyn::ratexp exp = make_ratexp(*ctx, ratexp);

  // Process.

  // Output.
  // FIXME: oneliner.
  vcsn::abstract_ratexpset* ratexpset = vcsn::dyn::make_ratexpset(*ctx);
  std::cout << ratexpset->format(exp->ratexp()) << std::endl;
}
