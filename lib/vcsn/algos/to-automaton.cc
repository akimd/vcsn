#include <vcsn/algos/derived-term.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/algos/strip.hh>
#include <vcsn/algos/thompson.hh>
#include <vcsn/algos/zpc.hh>

namespace vcsn
{
  namespace dyn
  {
    automaton
    to_automaton(const expression& exp, const std::string& algo)
    {
      if (algo == "auto" || algo == "derived_term" || algo == "derived-term")
        return strip(derived_term(exp));
      else if (algo == "standard")
        return standard(exp);
      else if (algo == "thompson")
        return thompson(exp);
      else if (algo == "zpc")
        return zpc(exp);
      else if (algo == "zpc_compact" || "zpc-compact" || "zpc,compact")
        return zpc(exp, "compact");
      else
        raise("to_automaton: invalid argument: ", algo, ", expected \"auto\", "
              "\"derived_term\", \"standard\", \"thompson\", \"zpc\", "
              "\"zpc_compact\" or nothing.");
    }
  }
}
