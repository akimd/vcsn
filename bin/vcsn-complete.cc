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
    opts.input_format = vcsn::dyn::FileType::text;
    parse_args(opts, argc, argv);

    if (!opts.is_automaton)
      throw std::logic_error("error: input must be a graph");

    using namespace vcsn::dyn;
    automaton aut = read_automaton(opts);

    aut = complete(aut);
    print (opts, aut);
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
