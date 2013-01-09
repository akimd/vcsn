#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include <vcsn/dyn/ratexp.hh>

#include <lib/vcsn/rat/driver.hh>
#include "parse-args.hh"

void
work_aut(options opts)
{
  using namespace vcsn::dyn;
  // Input.
  automaton aut = read_automaton_file(opts.file);
  // Process.
  auto res = transpose(aut);
  // Output.
  print(res, std::cout, opts.output_format) << std::endl;;
}

void
work_exp(options opts)
{
  // Input.
  using namespace vcsn::dyn;
  context* ctx = make_context(opts.context, opts.labelset_describ);
  ratexp exp = read_ratexp_file(opts.file, *ctx, opts.input_format);

  // Process.
  exp = transpose(exp);

  // Output.
  print(exp, std::cout, opts.output_format) << std::endl;
}

int main(int argc, char* const argv[])
try
  {
    auto opts = parse_args(argc, argv);
    if (opts.is_automaton)
      work_aut(opts);
    else
      work_exp(opts);
  }
 catch (const std::exception& e)
   {
     std::cerr << e.what() << std::endl;
     exit(EXIT_FAILURE);
   }
