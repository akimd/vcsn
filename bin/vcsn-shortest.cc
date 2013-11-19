#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/polynomial.hh>

#include "parse-args.hh"

struct shortest: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;

    // Input.
    auto aut = read_automaton(opts);
    unsigned num = (0 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[0])
                    : 1);

    // Process.
    auto res = vcsn::dyn::shortest(aut, num);

    // Output.
    if (!res->empty() || vcsn::dyn::get_format(*opts.out) != "list")
      *opts.out << res << std::endl;
    return 0;
  }

  int work_exp(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto exp = read_ratexp(opts);
    unsigned num = (0 < opts.argv.size()
                    ? boost::lexical_cast<unsigned>(opts.argv[0])
                    : 1);

    // Process.
    auto res = vcsn::dyn::shortest(standard(exp), num);

    // Output.
    if (!res->empty() || vcsn::dyn::get_format(*opts.out) != "list")
      *opts.out << res << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, shortest{});
}
