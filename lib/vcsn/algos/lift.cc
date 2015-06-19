#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {

    /*-------------------------.
    | lift(automaton, tapes).  |
    `-------------------------*/

    REGISTER_DEFINE(lift_automaton);
    automaton
    lift(const automaton& aut, const std::vector<unsigned>& tapes, vcsn::rat::identities ids)
    {
      std::string signame;
      for (auto t : tapes)
      {
        if (!signame.empty())
          signame += ", ";
        signame += "std::integral_constant<unsigned, " + std::to_string(t) + '>';
      }
      auto t = integral_constant{symbol("const std::tuple<" + signame + ">&")};
      signature sig;
      sig.sig.emplace_back(vname(aut));
      sig.sig.emplace_back("vcsn::rat::identities");
      for (const auto& t: tapes)
        sig.sig.emplace_back("std::integral_constant<unsigned, " + std::to_string(t) + '>');
      return detail::lift_automaton_registry().call(sig, aut, ids, t);
    }
  }
}
