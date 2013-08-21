#ifndef VCSN_ALGOS_CHAIN_HH
# define VCSN_ALGOS_CHAIN_HH

# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/algos/accessible.hh> // dyn::make_automaton
# include <vcsn/algos/concatenate.hh>

namespace vcsn
{
  /*-------.
  | chain  |
  `-------*/

  template <class Aut>
  Aut
  chain(const Aut& aut, size_t n)
  {
    switch (n)
    {
      case 0:
        {
          Aut res(aut.context());
          auto s = res.new_state();
          res.set_initial(s);
          res.set_final(s);
          return res;
        }
      case 1:
        {
          return accessible(aut);
        }
      default:
        {
          auto t = chain(aut, n / 2);
          auto t2 = concatenate(t, t);
          if (n % 2 == 0)
            // n is even.
            return t2;
          else
            // n is odd.
            return concatenate(t2, aut);
        }
    }
  }

  namespace dyn
  {
    namespace detail
    {
      /*-------------.
      | dyn::chain.  |
      `-------------*/

      template <typename Aut>
      automaton
      chain(const automaton& a, size_t n)
      {
        const auto& aut = dynamic_cast<const Aut&>(*a);
        return make_automaton(aut.context(), chain(aut, n));
      }

      REGISTER_DECLARE(chain,
                       (const automaton& aut, size_t n) -> automaton);
    }
  }
}

#endif // !VCSN_ALGOS_CHAIN_HH
