#pragma once

#include <set>

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/is-complete.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{
  /*------------------------------.
  | Function tag and properties.  |
  `------------------------------*/

  CREATE_FUNCTION_TAG(complement);

  template <>
  struct function_prop<complement_ftag>
  {
    static const bool invalidate = true;

    static auto& ignored_prop()
    {
      static auto ignored = ignored_prop_t{};
      return ignored;
    }

    static auto& updated_prop()
    {
#if defined __GNUC__ && ! defined __clang__
      // GCC 4.9 and 5.0 warnings: see
      // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
      static auto updated_prop = create_updated_prop(
        {
          // By construction, the automaton is deterministic (see notebook).
          { is_deterministic_ptag::id(), boost::any{true} }
        });
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
      return updated_prop;
    }
  };

  /*------------------------.
  | complement(automaton).  |
  `------------------------*/

  template <Automaton Aut>
  void
  complement_here(Aut& aut)
  {
    using automaton_t = Aut;
    static_assert(labelset_t_of<automaton_t>::is_free(),
                  "complement: requires free labelset");
    require(is_deterministic(aut),
            "complement: requires a deterministic automaton");
    require(is_complete(aut),
            "complement: requires a complete automaton");

    using state_t = state_t_of<automaton_t>;

    // The final states of aut.
    auto finals = std::set<state_t>{};
    for (auto t: final_transitions(aut))
      finals.insert(aut->src_of(t));

    // Complement.
    for (auto s: aut->states())
      if (has(finals, s))
        aut->unset_final(s);
      else
        aut->set_final(s);

    aut->properties().update(complement_ftag{});
  }

  template <Automaton Aut>
  auto
  complement(const Aut& aut)
    -> decltype(copy(aut))
  {
    auto res = copy(aut);
    complement_here(res);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton
      complement(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return ::vcsn::complement(a);
      }
    }
  }

  /*-------------------------.
  | complement(expansion).   |
  `-------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (complement).
      template <typename ExpansionSet>
      expansion
      complement_expansion(const expansion& xpn)
      {
        const auto& x = xpn->as<ExpansionSet>();
        return {x.valueset(),  x.valueset().complement(x.value())};
      }
    }
  }


  /*--------------------------.
  | complement(expression).   |
  `--------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (complement).
      template <typename ExpSet>
      expression
      complement_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return {e.valueset(),
                e.valueset().complement(e.value())};
      }
    }
  }
}
