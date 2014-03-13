#include <iostream>

#include <boost/lexical_cast.hpp>
#include <vcsn/dyn/algos.hh>
#include "parse-args.hh"

int main(int argc, char * const argv[])
try
  {
    options opts;
    opts.input_format = "text";
    parse_args(opts, argc, argv);

    // Input.
    using namespace vcsn::dyn;
    auto ctx = make_context(opts.context);
    assert(2 <= argc);
    auto k = boost::lexical_cast<unsigned>(argv[0]);
    auto b = boost::lexical_cast<unsigned>(argv[1]);

    // Process.
    automaton aut = divkbaseb(ctx, k, b);

    // Output.
    opts.print(aut);
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
