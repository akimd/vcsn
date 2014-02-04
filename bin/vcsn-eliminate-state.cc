#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct eliminate_state: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    int n = (0 < opts.argv.size()
             ? boost::lexical_cast<int>(opts.argv[0])
             : -1);

    // Process.
    auto res = vcsn::dyn::eliminate_state(aut, n);

    // Output.
    *opts.out << sort(res) << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, eliminate_state{});
}
