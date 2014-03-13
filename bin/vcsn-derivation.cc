#include <cassert>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct derivation: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);
    assert(0 < opts.argv.size());
    std::string s = opts.argv[0];
    bool breaking = (1 < opts.argv.size()
                     ? boost::lexical_cast<bool>(opts.argv[1])
                     : false);

    // Process.
    auto res = vcsn::dyn::derivation(exp, s, breaking);

    // Output.
    opts.print(res);
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, derivation{}, type::ratexp);
}
