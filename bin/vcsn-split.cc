#include <cassert>
#include <iostream>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct split: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);

    // Process.
    auto res = vcsn::dyn::split(exp);

    // Output.
    *opts.out << res << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, split{}, type::ratexp);
}
