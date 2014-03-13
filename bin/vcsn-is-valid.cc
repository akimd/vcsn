#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct is_valid: vcsn_function
{
  int work_aut(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    automaton aut = read_automaton(opts);

    // Process.
    bool res = vcsn::dyn::is_valid(aut);

    // Output.
    opts.print(res);
    return res ? 0 : 2;
  }

  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);

    // Process.
    bool res = vcsn::dyn::is_valid(exp);

    // Output.
    opts.print(res);
    return res ? 0 : 2;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, is_valid{});
}
