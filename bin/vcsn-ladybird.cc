#include <iostream>
#include <cassert>

#include <boost/lexical_cast.hpp>
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
    auto ctx = vcsn::dyn::make_context(opts.context, opts.gens);
    size_t n = boost::lexical_cast<size_t>(argv[0]);

    // Process.
    automaton aut = ladybird(ctx, n);

    // Output.
    print(aut, std::cout, opts.output_format) << std::endl;
  }
 catch (const std::exception& e)
   {
     std::cerr << e.what() << std::endl;
     exit(EXIT_FAILURE);
   }
