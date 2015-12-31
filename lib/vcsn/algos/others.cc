#include <lib/vcsn/algos/registry.hh>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/label.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/dyn/registries.hh>
#include <vcsn/dyn/signature-printer.hh>
#include <vcsn/dyn/translate.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{
  namespace dyn
  {

    /*---------------.
    | conjunction.   |
    `---------------*/

    // Implement the binary case on top of the variadic one, to avoid
    // compiling it twice.
    automaton
    conjunction(const automaton& lhs, const automaton& rhs, bool lazy)
    {
      auto auts = std::vector<automaton>{lhs, rhs};
      return conjunction(auts, lazy);
    }


    /*---------.
    | focus.   |
    `---------*/

    static
    integral_constant to_integral_constant(unsigned tape)
    {
      return integral_constant{symbol("std::integral_constant<unsigned, "
                                      + std::to_string(tape) + '>')};
    }

    REGISTRY_DEFINE(focus);
    automaton
    focus(const automaton& aut, unsigned tape)
    {
      auto t = to_integral_constant(tape);
      return detail::focus_registry().call(aut, t);
    }


    /*----------------.
    | infiltration.   |
    `----------------*/

    automaton
    infiltration(const automaton& lhs, const automaton& rhs)
    {
      auto auts = std::vector<automaton>{lhs, rhs};
      return infiltration(auts);
    }


    /*-------------------------.
    | lift(automaton, tapes).  |
    `-------------------------*/

    REGISTRY_DEFINE(lift_automaton);
    automaton
    lift(const automaton& aut,
         const std::vector<unsigned>& tapes, vcsn::rat::identities ids)
    {
      std::string signame;
      for (auto t : tapes)
      {
        if (!signame.empty())
          signame += ", ";
        signame += ("std::integral_constant<unsigned, "
                    + std::to_string(t) + '>');
      }
      auto t = integral_constant{symbol("const std::tuple<" + signame + ">&")};
      signature sig;
      sig.sig.emplace_back(vname(aut));
      sig.sig.emplace_back("vcsn::rat::identities");
      for (const auto& t: tapes)
        sig.sig.emplace_back("std::integral_constant<unsigned, "
                             + std::to_string(t) + '>');
      return detail::lift_automaton_registry().call(sig, aut, ids, t);
    }


    /*---------------.
    | make_context.  |
    `---------------*/

    REGISTRY_DEFINE(make_context);
    context
    make_context(const std::string& n)
    {
      symbol sname{ast::normalize_context(n, false)};
      std::string full_name = ast::normalize_context(n, true);
      if (!detail::make_context_registry().get0({sname}))
        compile(sname);
      return detail::make_context_registry().call({sname}, full_name);
    }


    /*---------.
    | project. |
    `---------*/

    REGISTRY_DEFINE(project);
    automaton
    project(const automaton& aut, unsigned tape)
    {
      auto t = to_integral_constant(tape);
      return detail::project_registry().call(aut, t);
    }

    REGISTRY_DEFINE(project_context);
    context
    project(const context& ctx, unsigned tape)
    {
      auto t = to_integral_constant(tape);
      return detail::project_context_registry().call(ctx, t);
    }

    REGISTRY_DEFINE(project_polynomial);
    polynomial
    project(const polynomial& p, unsigned tape)
    {
      auto t = to_integral_constant(tape);
      return detail::project_polynomial_registry().call(p, t);
    }

    REGISTRY_DEFINE(project_label);
    label
    project(const label& l, unsigned tape)
    {
      auto t = to_integral_constant(tape);
      return detail::project_label_registry().call(l, t);
    }

    /*-----------.
    | shuffle.   |
    `-----------*/

    automaton
    shuffle(const automaton& lhs, const automaton& rhs)
    {
      auto auts = std::vector<automaton>{lhs, rhs};
      return shuffle(auts);
    }


    /*----------------.
    | to_automaton.   |
    `----------------*/

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
          return strip(trim(strip(derived_term(exp))));
        case algo_t::derivation:
          return strip(trim(strip(derived_term(exp, "derivation"))));
        case algo_t::standard:
          return standard(exp);
        case algo_t::thompson:
          return thompson(exp);
        case algo_t::zpc:
          return strip(trim(zpc(exp)));
        case algo_t::zpc_compact:
          return strip(trim(zpc(exp, "compact")));
        }
      BUILTIN_UNREACHABLE();
    }


    /*---------.
    | tuple.   |
    `---------*/

    expression
    tuple(const expression& lhs, const expression& rhs)
    {
      auto auts = std::vector<expression>{lhs, rhs};
      return tuple(auts);
    }

    std::string type(const automaton& a)
    {
      return a->vname();
    }
  }
}
