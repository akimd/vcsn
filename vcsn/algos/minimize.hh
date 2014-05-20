#ifndef VCSN_ALGOS_MINIMIZE_HH
# define VCSN_ALGOS_MINIMIZE_HH

# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/algos/minimize-brzozowski.hh>
# include <vcsn/algos/minimize-moore.hh>
# include <vcsn/algos/minimize-signature.hh>
# include <vcsn/algos/minimize-weighted.hh>
# include <vcsn/dyn/automaton.hh>

namespace vcsn
{

  // FIXME: there must exist something nicer to do that.

  template <typename Aut>
  inline
  typename std::enable_if<std::is_same<weight_t_of<Aut>, bool>::value,
                          Aut>::type
  minimize(const Aut& a, const std::string& algo)
  {
    if (algo == "brzozowski")
      return minimize_brzozoski(a);
    else if (algo == "moore")
      return minimize_moore(a);
    else if (algo == "signature")
      return minimize_signature(a);
    else if (algo == "weighted")
      return minimize_weighted(a); // FIXME: fix
    else
      raise("minimize: invalid algorithm: ", str_escape(algo));
  }

  template <typename Aut>
  inline
  typename std::enable_if<!std::is_same<weight_t_of<Aut>, bool>::value,
                          Aut>::type
  minimize(const Aut& a, const std::string& algo)
  {
    if (algo == "weighted")
      return minimize_weighted(a); // FIXME: fix
    else
      raise("minimize: invalid algorithm: ", str_escape(algo));
  }


  /*----------------.
  | dyn::minimize.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {

      template <typename Aut, typename String>
      automaton
      minimize(const automaton& aut, const std::string& algo)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(minimize(a, algo));
      }

      REGISTER_DECLARE
      (minimize,
       (const automaton& aut, const std::string& algo) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_MINIMIZE_HH
