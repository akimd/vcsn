#include <cassert>
#include <iostream>
#include <string>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct left_mult: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    automaton aut = read_automaton(opts);
    assert(1 <= opts.argv.size());

    // FIXME: hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    opts2.input_is_file = false;
    weight w = read_weight(opts2);

    // Process.
    auto res = vcsn::dyn::left_mult(aut, w);

    // Output.
    *opts.out << sort(res) << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, left_mult{});
}
