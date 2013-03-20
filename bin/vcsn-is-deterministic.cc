#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>
#include "parse-args.hh"

int
main(int argc, char* const argv[])
try
  {
    options opts;
    opts.is_automaton = false;
    opts.input_format = "text";
    parse_args(opts, argc, argv);

    if (!opts.is_automaton)
      throw std::logic_error("error: input must be an automaton");

    using namespace vcsn::dyn;
    automaton aut = read_automaton(opts);

    std::cout << std::boolalpha
              << is_deterministic(aut)
              << std::endl;
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
