#ifndef VCSN_ALGOS_STRIP_HH
# define VCSN_ALGOS_STRIP_HH

# include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  namespace detail
  {
    // Rely on the fact that int takes precedence over long to express
    // a precedence of this first function over the second one.

    // automata that feature a strip member function.
    template <typename Aut>
    inline
    auto
    strip(const Aut& aut, int)
      -> decltype(aut->strip())
    {
      return aut->strip();
    }

    // automata that don't feature a strip member function.
    template <typename Aut>
    inline
    auto
    strip(const Aut& aut, long)
      -> decltype(aut)
    {
      return aut;
    }
  }

  /// Remove (all) the decorations from a decorated automaton.
  template <typename Aut>
  inline
  auto
  strip(const Aut& aut)
    -> decltype(detail::strip(aut, 0))
  {
    return detail::strip(aut, 0);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      inline
      automaton
      strip(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::strip(a));
      }

      REGISTER_DECLARE(strip, (const automaton& a) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_STRIP_HH
