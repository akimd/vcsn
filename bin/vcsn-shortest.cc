#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct shortest: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);

    // Process.
    auto res = vcsn::dyn::shortest(aut);

    // Output.
    std::cout << res << std::endl;
    return 0;
  }

  int work_exp(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_ratexp(opts);

    // Process.
    auto res = vcsn::dyn::shortest(standard(exp));

    // Output.
    std::cout << res << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, shortest{});
}
