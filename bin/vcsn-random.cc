#include <iostream>

#include <boost/lexical_cast.hpp>
#include <vcsn/dyn/algos.hh>
#include "parse-args.hh"

int main(int argc, char * const argv[])
try
  {
    options opts;
    parse_args(opts, argc, argv);

    // Input.
    using namespace vcsn::dyn;
    auto ctx = vcsn::dyn::make_context(opts.context);
    assert(0 < argc);
    assert(argc <= 4);
    unsigned num_states  =          boost::lexical_cast<unsigned>(argv[0]);
    float density        = 1 < argc ? boost::lexical_cast<float>(argv[1])    : .1;
    unsigned num_initial = 2 < argc ? boost::lexical_cast<unsigned>(argv[2]) : 1;
    unsigned num_final   = 3 < argc ? boost::lexical_cast<unsigned>(argv[3]) : 1;

    // Process.
    automaton aut = random_automaton(ctx, num_states, density,
                                     num_initial, num_final);

    // Output.
    *opts.out << aut << std::endl;
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
