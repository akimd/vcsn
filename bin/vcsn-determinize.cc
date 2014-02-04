#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct determinize: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    bool complete = (0 < opts.argv.size()
                     ? boost::lexical_cast<bool>(opts.argv[0])
                     : false);

    // Process.
    auto res = vcsn::dyn::determinize(aut, complete);

    // Output.
    *opts.out << sort(res) << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, determinize{});
}
