#ifndef VCSN_ALGOS_IDENTITIES_HH
# define VCSN_ALGOS_IDENTITIES_HH

# include <map>

# include <vcsn/algos/product.hh> // join_automata
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp
# include <vcsn/misc/raise.hh> // require

namespace vcsn
{

  /*---------------------.
  | identities(ratexp).  |
  `---------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      rat::identities
      identities(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        auto rs = e.ratexpset();
        return rs.identities();
      }

      REGISTER_DECLARE(identities,
                       (const ratexp&) -> rat::identities);
    }

  }
}

#endif // !VCSN_ALGOS_IDENTITIES_HH
