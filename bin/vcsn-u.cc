#include <iostream>

#include <boost/lexical_cast.hpp>
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
    auto ctx = vcsn::dyn::make_context(opts.context);
    assert(1 <= argc);
    auto n = boost::lexical_cast<unsigned>(argv[0]);

    // Process.
    automaton aut = u(ctx, n);

    // Output.
    print(opts, aut);
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
