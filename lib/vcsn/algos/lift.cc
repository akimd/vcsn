#include <lib/vcsn/algos/registry.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/registers.hh>

namespace vcsn
{
  namespace dyn
  {

    /*------------------.
    | lift(automaton).  |
    `------------------*/

    REGISTER_DEFINE(lift_automaton);
    automaton
    lift(const automaton& aut)
    {
      return detail::lift_automaton_registry().call(aut);
    }

    /*-------------------------.
    | lift(automaton, tapes).  |
    `-------------------------*/

    REGISTER_DEFINE(lift_automaton_tape);
    automaton
    lift(const automaton& aut, const std::vector<unsigned>& tapes)
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
      for (const auto& t: tapes)
        sig.sig.emplace_back("std::integral_constant<unsigned, " + std::to_string(t) + '>');
      return detail::lift_automaton_tape_registry().call(sig, aut, t);
    }
  }
}
