#include <iostream>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct enumerate: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    assert(1 <= opts.argv.size());
    size_t n = boost::lexical_cast<size_t>(opts.argv[0]);

    // Process.
    auto res = vcsn::dyn::enumerate(aut, n);

    // Output.
    for (const auto&r: res)
      std::cout << r << std::endl;
    return 0;
  }

  int work_exp(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_ratexp(opts);
    size_t n = boost::lexical_cast<size_t>(opts.argv[0]);

    // Process.
    auto res = vcsn::dyn::enumerate(standard(exp), n);

    // Output.
    for (const auto&r: res)
      std::cout << r << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, enumerate{});
}
