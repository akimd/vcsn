#include <cassert>
#include <iostream>
#include <string>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct right_mult: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    assert(1 <= opts.argv.size());
    // Weight will be converted later.
    std::string w = opts.argv[0];

    // Process.
    auto res = vcsn::dyn::right_mult(aut, w);

    // Output.
    *opts.out << res << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, right_mult{});
}
