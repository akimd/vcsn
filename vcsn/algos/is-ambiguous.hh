#ifndef VCSN_ALGOS_IS_AMBIGUOUS_HH
# define VCSN_ALGOS_IS_AMBIGUOUS_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  template <typename Aut>
  bool is_ambiguous(const Aut& aut)
  {
    detail::producter<Aut, Aut> product;
    auto prod = product(aut, aut);
    // Check if there useful states outside of the diagonal.  Since
    // the product is accessible, check only for coaccessibles states.
    auto coaccessible = coaccessible_states(prod);
    for (const auto& o: product.origins())
      if (o.second.first != o.second.second
          && has(coaccessible, o.first))
        {
          if (getenv("VCSN_DEBUG"))
            std::cerr << "ambiguous: " << o.first - 2
                      << " (" << o.second.first - 2
                      << ", " << o.second.second - 2 << ")"
                      << std::endl;
          return true;
        }
    return false;
  }

  /*--------------------.
  | dyn::is_ambiguous.  |
  `--------------------*/

  namespace dyn
  {
    namespace detail
    {
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
