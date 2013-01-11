#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include "parse-args.hh"

int main(int argc, char * const argv[])
try
  {
    options opts = parse_args(argc, argv);

    // Input.
    using namespace vcsn::dyn;
    automaton exp = read_automaton(opts);

    // Process.
    ratexp aut = aut_to_exp(exp);

    // Output.
    print(aut, std::cout, opts.output_format) << std::endl;
  }
 catch (const std::exception& e)
   {
     std::cerr << e.what() << std::endl;
     exit(EXIT_FAILURE);
   }
