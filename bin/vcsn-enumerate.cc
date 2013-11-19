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
    unsigned max = (0 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[0])
                    : 1);

    // Process.
    auto res = vcsn::dyn::enumerate(aut, max);

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
    unsigned max = (0 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[0])
                    : 1);

    // Process.
    auto res = vcsn::dyn::enumerate(standard(exp), max);

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
