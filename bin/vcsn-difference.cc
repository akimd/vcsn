#include <cassert>
#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct difference: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    automaton lhs = read_automaton(opts);
    // FIXME: hack.
    assert(1 <= opts.argv.size());
    options opts2 = opts;
    opts2.input = opts.argv[0];
    automaton rhs = read_automaton(opts2);

    // Process.
    auto res = vcsn::dyn::difference(lhs, rhs);

    // Output.
    opts.print(res);
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, difference{});
}
