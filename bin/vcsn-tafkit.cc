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
      opts.print(res);                          \
      return 0;                                 \
    }                                           \
  }

#define DEFINE_AUTOMATON_SIZE_FUNCTION(Name)                    \
  struct Name: vcsn_function                                    \
  {                                                             \
    int work_aut(const options& opts) const                     \
    {                                                           \
      using namespace vcsn::dyn;                                \
      /* Input. */                                              \
      automaton aut = read_automaton(opts);                     \
      assert(1 <= opts.argv.size());                            \
      size_t n = boost::lexical_cast<size_t>(opts.argv[0]);     \
                                                                \
      /* Process. */                                            \
      auto res = vcsn::dyn::Name(aut, n);                       \
                                                                \
      /* Output. */                                             \
      opts.print(res);                                          \
      return 0;                                                 \
    }                                                           \
  }


DEFINE_AUTOMATON_FUNCTION(accessible);
DEFINE_AUTOMATON_FUNCTION(aut_to_exp);
DEFINE_AUTOMATON_SIZE_FUNCTION(chain);
DEFINE_AUTOMATON_FUNCTION(coaccessible);
DEFINE_AUTOMATON_SIZE_FUNCTION(power);

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
    opts.print(res);
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
    opts.print(res);
    return res ? 0 : 2;
  }
};

struct are_isomorphic: vcsn_function
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

    // FIXME: generalize to an arbitrary number of automata

    // Process.
    auto res = vcsn::dyn::are_isomorphic(lhs, rhs);

    // Output.
    opts.print(res);
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
    opts.print(res);
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
  ALGO(are_isomorphic);
  ALGO(aut_to_exp);
  ALGO(chain);
  ALGO(coaccessible);
  ALGO(determinize);
  ALGO(power);
  else
    vcsn::raise("unknown command: " + cmd);
 return vcsn_main(argc - 1, argv + 1, *f);
}
