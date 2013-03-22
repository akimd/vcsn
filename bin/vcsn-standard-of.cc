#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct standard_of: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);

    // Process.
    automaton aut = vcsn::dyn::standard_of(exp);

    // Output.
    print(opts, aut);
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, standard_of{}, false);
}
