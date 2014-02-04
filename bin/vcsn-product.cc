#include <cassert>
#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct product: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;

    // Input.
    automaton res = read_automaton(opts);

    // Process.
    for (unsigned i = 0; i < opts.argv.size(); ++i)
      {
        // FIXME: hack.
        options opts2 = opts;
        opts2.input = opts.argv[i];
        automaton rhs = read_automaton(opts2);
        res = vcsn::dyn::product(res, rhs);
      }

    // Output.
    *opts.out << sort(res) << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, product{});
}
