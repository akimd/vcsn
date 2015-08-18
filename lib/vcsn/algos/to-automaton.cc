#include <vcsn/algos/derived-term.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/algos/thompson.hh>
#include <vcsn/algos/zpc.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{
  namespace dyn
  {
    automaton
    to_automaton(const expression& exp, const std::string& algo)
    {
      enum class algo_t
      {
        derivation,
        expansion,
        standard,
        thompson,
        zpc,
        zpc_compact,
       };
      static const auto map = std::map<std::string, algo_t>
        {
          {"derivation",   algo_t::derivation},
          {"expansion",    algo_t::expansion},
          {"derived_term", algo_t::expansion},
          {"auto",         algo_t::expansion},
          {"standard",     algo_t::standard},
          {"thompson",     algo_t::thompson},
          {"zpc",          algo_t::zpc},
          {"zpc_compact",  algo_t::zpc_compact},
        };
      switch (getargs("to_automaton: algorithm", map, algo))
        {
        case algo_t::expansion:
          return strip(derived_term(exp));
        case algo_t::derivation:
          return strip(derived_term(exp, "derivation"));
        case algo_t::standard:
          return standard(exp);
        case algo_t::thompson:
          return thompson(exp);
        case algo_t::zpc:
          return zpc(exp);
        case algo_t::zpc_compact:
          return zpc(exp, "compact");
        }
      BUILTIN_UNREACHABLE();
    }
  }
}
