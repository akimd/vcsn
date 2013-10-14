#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct derived_terms: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);

    // Process.
    automaton res = vcsn::dyn::derived_terms(exp);

    // Output.
    *opts.out << res << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, derived_terms{}, type::ratexp);
}
