#include <cassert>
#include <iostream>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct derive: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);
    assert(1 <= opts.argv.size());
    std::string s = opts.argv[0];

    // Process.
    auto res = vcsn::dyn::derive(exp, s);

    // Output.
    *opts.out << res << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, derive{}, type::ratexp);
}
