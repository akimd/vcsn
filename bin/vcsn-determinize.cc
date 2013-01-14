#include <iostream>

#include <vcsn/algos/dyn.hh>
#include "parse-args.hh"

int main(int argc, char * const argv[])
try
  {
    options opts;
    opts.is_automaton = false;
    opts.input_format = vcsn::dyn::FileType::text;
    parse_args(opts, argc, argv);

    // Input.
    using namespace vcsn::dyn;
    automaton aut = read_automaton(opts);

    // Process.
    aut = determinize(aut);

    // Output.
    print(aut, std::cout, FileType::dot) << std::endl;
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
