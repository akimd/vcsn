#ifndef VCSN_ALGOS_INFO_HH
# define VCSN_ALGOS_INFO_HH

# include <iostream>
# include <sstream>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/core/rat/info.hh>

namespace vcsn
{

  /*--------------------------.
  | info(automaton, stream).  |
  `--------------------------*/

  template <class A>
  std::ostream&
  info(const A& aut, std::ostream& out)
  {
    return out
      << "context: " << aut.vname(true) << std::endl
      << "number of states: " << aut.num_states() << std::endl
      << "number of initial states: " << aut.num_initials() << std::endl
      << "number of final states: " << aut.num_finals() << std::endl
      << "number of transitions: " << aut.num_transitions() << std::endl
      //<< "is complete: " << is_complete(aut) << std::endl
      << "is deterministic: " << is_deterministic(aut) << std::endl
      << "is eps-acyclic: " << is_eps_acyclic(aut) << std::endl
      << "is proper: " << is_proper(aut) << std::endl
      << "is valid: " << is_valid(aut);
      ;
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      std::ostream& info(const automaton& aut, std::ostream& out)
      {
        info(dynamic_cast<const Aut&>(*aut), out);
        return out;
      }

      REGISTER_DECLARE(info,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }


  /*-----------------------.
  | info(ratexp, stream).  |
  `-----------------------*/

  template <class Ctx>
  void
  info(const Ctx& ctx, const rat::exp_t& e, std::ostream& o)
  {
    vcsn::rat::info<Ctx> nfo;
    nfo(*ctx.downcast(e));
# define DEFINE(Type)                            \
    << std::endl << #Type ": " << nfo.Type
    o
      << "context: " << ctx.vname(true)
      DEFINE(sum)
      DEFINE(prod)
      DEFINE(star)
      DEFINE(zero)
      DEFINE(one)
      DEFINE(atom)
      ;
# undef DEFINE
  }


  namespace dyn
  {
    namespace detail
    {
      /// Abstract but parameterized.
      template <typename Context>
      std::ostream& info_exp(const ratexp& exp, std::ostream& o)
      {
        const auto& ctx = dynamic_cast<const Context&>(exp->ctx());
        vcsn::info<Context>(ctx, exp->ratexp(), o);
        return o;
      }

      REGISTER_DECLARE(info_exp,
                       (const ratexp& aut, std::ostream& o) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_INFO_HH
