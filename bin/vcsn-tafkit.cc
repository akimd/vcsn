#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>
#include <vcsn/misc/raise.hh>

#include "parse-args.hh"

#define DEFINE_AUTOMATON_FUNCTION(Name)         \
  struct Name: vcsn_function                    \
  {                                             \
    int work_aut(const options& opts) const     \
    {                                           \
      using namespace vcsn::dyn;                \
      /* Input. */                              \
      auto aut = read_automaton(opts);          \
                                                \
      /* Process. */                            \
      auto res = vcsn::dyn::Name(aut);          \
                                                \
      /* Output. */                             \
      *opts.out << sort(res) << std::endl;      \
      return 0;                                 \
    }                                           \
  }

DEFINE_AUTOMATON_FUNCTION(accessible);
DEFINE_AUTOMATON_FUNCTION(coaccessible);

struct are_equivalent: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    automaton lhs = read_automaton(opts);
    // FIXME: hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    automaton rhs = read_automaton(opts2);

    // Process.
    auto res = vcsn::dyn::are_equivalent(lhs, rhs);

    // Output.
    *opts.out << res << std::endl;
    return res ? 0 : 2;
  }

  int work_exp(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    ratexp lhs = read_ratexp(opts);
    // FIXME: hack.
    options opts2 = opts;
    opts2.input = opts.argv[0];
    ratexp rhs = read_ratexp(opts2);

    // Process.
    auto res = vcsn::dyn::are_equivalent(lhs, rhs);

    // Output.
    *opts.out << res << std::endl;
    return res ? 0 : 2;
  }
};

struct determinize: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    bool complete = (0 < opts.argv.size()
                     ? boost::lexical_cast<bool>(opts.argv[0])
                     : false);

    // Process.
    auto res = vcsn::dyn::determinize(aut, complete);

    // Output.
    *opts.out << sort(res) << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  vcsn::require(1 < argc, "no command given");
  std::string cmd{argv[1]};
  std::unique_ptr<vcsn_function> f;
#define ALGO(Name)                              \
  else if (cmd == #Name) f = std::unique_ptr<Name>(new Name{})
  if (false) {}
  ALGO(accessible);
  ALGO(are_equivalent);
  ALGO(coaccessible);
  ALGO(determinize);
  else
    vcsn::raise("unknown command: " + cmd);
 return vcsn_main(argc - 1, argv + 1, *f);
}
