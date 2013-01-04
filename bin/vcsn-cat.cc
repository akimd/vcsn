#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/rat/driver.hh>
#include "parse-args.hh"

int main(int argc, char * argv[])
{
  using namespace vcsn::dyn;

  auto opts = parse_args(&argc, &argv);

  assert (argc == 2);
  std::string ratexp_str = argv[1];

  // Input.
  vcsn::dyn::context* ctx = make_context(opts.context, opts.labelset_describ);
  vcsn::dyn::ratexp exp = read_ratexp_string(ratexp_str, *ctx,
                                             opts.input_format);

  // Output.
  print(exp, std::cout, opts.output_format) << std::endl;
}
