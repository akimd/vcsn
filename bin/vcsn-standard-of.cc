#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include "parse-args.hh"

int main(int argc, char * argv[])
{
  auto opts = parse_args(&argc, &argv);
  assert(argc == 2);

  // Input.
  using namespace vcsn::dyn;
  vcsn::dyn::context* ctx = make_context(opts.context, opts.labelset_describ);
  vcsn::dyn::ratexp exp =read_ratexp_string(argv[1], *ctx, opts.input_format);

  // Process.
  vcsn::dyn::automaton aut = standard_of(exp);

  // Output.
  print(aut, std::cout, opts.output_format) << std::endl;
}
