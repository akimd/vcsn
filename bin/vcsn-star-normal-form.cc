#include <iostream>
#include <stdexcept>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct star_normal_form: vcsn_function
{
  int work_exp(const options& opts) const
  {
    // Input.
    using namespace vcsn::dyn;
    ratexp exp = read_ratexp(opts);

    // Process.
    ratexp res = vcsn::dyn::star_normal_form(exp);

    // Output.
    opts.print(res);
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, star_normal_form{}, type::ratexp);
}
