#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct is_complete: vcsn_function
{
  static int
  work_aut(const options& opts)
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);

    // Process.
    bool res = vcsn::dyn::is_complete(aut);

    // Output.
    std::cout << (res ? "true" : "false") << std::endl;
    return res ? 0 : 2;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, is_complete{});
}
