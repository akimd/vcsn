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
    automaton lhs = read_automaton(opts);
    // FIXME: hack.
    std::string input2 = argv[0];
    std::swap(input2, opts.input);
    automaton rhs = read_automaton(opts);
    std::swap(input2, opts.input);

    // Process.
    auto res = product(lhs, rhs);

    // Output.
    print(opts, res);
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
