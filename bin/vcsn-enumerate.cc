#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/polynomial.hh>

#include "parse-args.hh"

struct enumerate: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // FIXME: Not perfectly elegant.
    if (opts.output_format == "default" || opts.output_format == "")
      vcsn::dyn::set_format(*opts.out, "list");

    // Input.
    auto aut = read_automaton(opts);
    assert(1 <= opts.argv.size());
    size_t n = boost::lexical_cast<size_t>(opts.argv[0]);

    // Process.
    auto res = vcsn::dyn::enumerate(aut, n);

    // Output.
    if (!res->empty() || vcsn::dyn::get_format(*opts.out) != "list")
      *opts.out << res << std::endl;
    return 0;
  }

  int work_exp(const options& opts) const
  {
    using namespace vcsn::dyn;
    // FIXME: Not perfectly elegant.
    if (opts.output_format == "default" || opts.output_format == "")
      vcsn::dyn::set_format(*opts.out, "list");

    // Input.
    auto exp = read_ratexp(opts);
    size_t n = boost::lexical_cast<size_t>(opts.argv[0]);

    // Process.
    auto res = vcsn::dyn::enumerate(standard(exp), n);

    // Output.
    if (!res->empty() || vcsn::dyn::get_format(*opts.out) != "list")
      *opts.out << res << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, enumerate{});
}
