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
    auto ctx = vcsn::dyn::make_context(opts.context);
    assert(2 <= argc);
    unsigned num_states = boost::lexical_cast<unsigned>(argv[0]);
    float density = boost::lexical_cast<float>(argv[1]);

    // Process.
    automaton aut = random_automaton(ctx, num_states, density);

    // Output.
    *opts.out << aut << std::endl;
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
