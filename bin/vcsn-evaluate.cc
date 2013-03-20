#include <iostream>

#include <vcsn/dyn/algos.hh>
#include "parse-args.hh"

int main(int argc, char * const argv[])
try
  {
    options opts;
    opts.is_automaton = false;
    opts.input_format = "text";
    parse_args(opts, argc, argv);

    // Input.
    using namespace vcsn::dyn;
    automaton aut = read_automaton(opts);

    // Process.
    auto res = eval(aut, argv[0]);

    // Output.
    std::cout << res << std::endl;
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
