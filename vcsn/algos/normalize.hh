#ifndef VCSN_ALGOS_NORMALIZE_HH
# define VCSN_ALGOS_NORMALIZE_HH

# include <vcsn/algos/standard.hh>
# include <vcsn/dyn/algos.hh>
# include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /// Whether \a a is standard and co-standard.
  template <typename Aut>
  bool
  is_normalized(const Aut& a)
  {
    return is_standard(a) && is_costandard(a);
  }

  /// Normalize \a a automaton.
  template <typename Aut>
  auto
  normalize(const Aut& a)
    -> decltype(copy(a))
  {
    return costandard(standard(a));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      bool
      is_normalized(const automaton& aut)
      {
        return is_normalized(aut->as<Aut>());
      }

      /// Bridge.
      template <typename Aut>
      automaton
      normalize(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::normalize(a));
      }
    }
  }
} // vcsn::

#endif // !VCSN_ALGOS_NORMALIZE_HH
