#ifndef VCSN_ALGOS_IS_PROPER_HH
# define VCSN_ALGOS_IS_PROPER_HH

# include <type_traits>

# include <vcsn/core/kind.hh>
# include <vcsn/dyn/automaton.hh> // make_automaton

namespace vcsn
{

  namespace detail
  {
    template <typename Aut>
    typename std::enable_if<!Aut::context_t::is_lal,
                            bool>::type
    is_proper_(const Aut& aut)
    {
      for (auto t: aut.transitions())
        if (aut.labelset()->is_one(aut.label_of(t)))
          return false;
      return true;
    }

    template <typename Aut>
    constexpr
    typename std::enable_if<Aut::context_t::is_lal,
                            bool>::type
    is_proper_(const Aut&)
    {
      return true;
    }
  }

  /**@brief Test whether an automaton is proper.

     An automaton is proper iff it contains no epsilon-transition.

     @param aut The tested automaton
     @return true iff the automaton is proper
  */
  template <typename Aut>
  bool
  is_proper(const Aut& aut)
  {
    return detail::is_proper_(aut);
  }

  namespace dyn
  {

    /*-----------------.
    | dyn::is_proper.  |
    `-----------------*/

    namespace detail
    {
      template <typename Aut>
      bool is_proper(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return is_proper(a);
      }

     REGISTER_DECLARE(is_proper,
                      (const automaton& aut) -> bool);
    }

  }

} // namespace vcsn

#endif // !VCSN_ALGOS_IS_PROPER_HH
