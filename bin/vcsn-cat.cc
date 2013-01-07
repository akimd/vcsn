#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/rat/driver.hh>
#include "parse-args.hh"

static void
work_aut(const options& opts)
{
  using namespace vcsn::dyn;
  // Input.
  auto aut = read_automaton_file(opts.file);
  // Output.
  print(aut, std::cout, opts.output_format) << std::endl;
}

static void
work_exp(const options& opts)
{
  using namespace vcsn::dyn;
  // Input.
  auto ctx = make_context(opts.context, opts.labelset_describ);
  auto exp = read_ratexp_file(opts.file, *ctx, opts.input_format);

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
