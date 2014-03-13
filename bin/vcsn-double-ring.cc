#include <iostream>
#include <vector>

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
    assert(1 <= argc);
    size_t n = boost::lexical_cast<size_t>(argv[0]);
    // final states.
    std::vector<unsigned> f;
    for (int i = 1; i < argc; ++i)
      f.emplace_back(boost::lexical_cast<unsigned>(argv[i]));

    // Process.
    automaton aut = double_ring(ctx, n, f);

    // Output.
    opts.print(aut);
  }
catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
