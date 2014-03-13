#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct standard: vcsn_function
{
  int work_aut(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    automaton aut = read_automaton(opts);

    // Process.
    automaton res = vcsn::dyn::standard(aut);

    // Output.
    opts.print(res);
    return 0;
  }

  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);

    // Process.
    automaton res = vcsn::dyn::standard(exp);

    // Output.
    opts.print(res);
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, standard{}, type::ratexp);
}
