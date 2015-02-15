#ifndef VCSN_ALGOS_IDENTITIES_HH
# define VCSN_ALGOS_IDENTITIES_HH

# include <map>

# include <vcsn/algos/product.hh> // join_automata
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/expression.hh> // dyn::make_expression
# include <vcsn/misc/raise.hh> // require

namespace vcsn
{

  /*---------------------.
  | identities(expression).  |
  `---------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      rat::identities
      identities(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        auto rs = e.expressionset();
        return rs.identities();
      }
    }

  }
}

#endif // !VCSN_ALGOS_IDENTITIES_HH
