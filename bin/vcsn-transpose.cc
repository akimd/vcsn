#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct transpose: vcsn_function
{
  virtual int work_aut(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);

    // Process.
    auto res = vcsn::dyn::transpose(aut);

    // Output.
    print(opts, res);
    return 0;
  }

  virtual int work_exp(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_ratexp(opts);

    // Process.
    auto res = vcsn::dyn::transpose(exp);

    // Output.
    print(opts, res);
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, transpose{});
}
