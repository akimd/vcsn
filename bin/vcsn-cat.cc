#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct cat: vcsn_function
{
  virtual int work_aut(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    // Output.
    opts.print(aut);
    return 0;
  }

  virtual int work_exp(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_ratexp(opts);
    // Output.
    opts.print(exp);
    return 0;
  }

  virtual int work_polynomial(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_polynomial(opts);
    // Output.
    opts.print(exp);
    return 0;
  }

  virtual int work_weight(const options& opts) const override
  {
    using namespace vcsn::dyn;
    // Input.
    auto w = read_weight(opts);
    // Output.
    opts.print(w);
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, cat{});
}
