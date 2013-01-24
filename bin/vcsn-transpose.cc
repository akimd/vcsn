#include <iostream>

#include <vcsn/algos/dyn.hh>
#include "parse-args.hh"

void
work_aut(options opts)
{
  using namespace vcsn::dyn;
  // Input.
  automaton aut = read_automaton(opts);
  // Process.
  auto res = transpose(aut);
  // Output.
  print(opts, res);
}

void
work_exp(options opts)
{
  using namespace vcsn::dyn;
  // Input.
  ratexp exp = read_ratexp(opts);

  // Process.
  exp = transpose(exp);

  // Output.
  print(opts, exp);
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
