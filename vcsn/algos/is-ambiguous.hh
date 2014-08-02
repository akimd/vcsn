#ifndef VCSN_ALGOS_IS_AMBIGUOUS_HH
# define VCSN_ALGOS_IS_AMBIGUOUS_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*---------------.
  | is_ambiguous.  |
  `---------------*/

  /// Whether an automaton is ambiguous.
  ///
  /// \param in aut        the automaton
  /// \returns whether ambiguous.
  template <typename Aut>
  bool is_ambiguous(const Aut& aut)
  {
    auto prod = product(aut, aut);
    // Check if there useful states outside of the diagonal.  Since
    // the product is accessible, check only for coaccessibles states.
    auto coaccessible = coaccessible_states(prod);
    for (const auto& o: prod->origins())
      if (std::get<0>(o.second) != std::get<1>(o.second)
          && has(coaccessible, o.first))
        {
          if (getenv("VCSN_DEBUG"))
            std::cerr << "ambiguous: " << o.first - 2
                      << " (" << std::get<0>(o.second) - 2
                      << ", " << std::get<1>(o.second) - 2 << ")"
                      << std::endl;
          return true;
        }
    return false;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <class Aut>
      bool is_ambiguous(const automaton& aut)
      {
        return is_ambiguous(aut->as<Aut>());
      }

      REGISTER_DECLARE(is_ambiguous,
                       (const automaton&) -> bool);
    }
  }
}

#endif // !VCSN_ALGOS_IS_AMBIGUOUS_HH
