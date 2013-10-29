#ifndef VCSN_DYN_AUTOMATON_HH
# define VCSN_DYN_AUTOMATON_HH

# include <memory> // shared_ptr
# include <string>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Base class for automata.
      ///
      /// FIXME: Should not exist, we should model as we did for ratexp.
      class LIBVCSN_API automaton_base
      {
      public:
        /// Base class for automata.
        virtual ~automaton_base() = default;

        /// A description of the automaton, sufficient to build it.
        /// \param full  whether to include the genset.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        template <typename Aut>
        Aut& as()
        {
          return dynamic_cast<Aut&>(*this);
        }

        template <typename Aut>
        const Aut& as() const
        {
          return dynamic_cast<const Aut&>(*this);
        }
      };
    }

    using automaton = std::shared_ptr<detail::automaton_base>;


    /// Build a dyn::automaton.
    ///
    /// FIXME: Eventually, we will aggregate the context on the
    /// side, as for ratexps.
    template <typename AutIn, typename AutOut = AutIn,
              typename Ctx = typename AutOut::context_t>
    inline
    automaton
    make_automaton(const Ctx&, AutIn& aut)
    {
      return std::make_shared<AutOut>(aut);
    }

    template <typename AutIn, typename AutOut = AutIn,
              typename Ctx = typename AutOut::context_t>
    inline
    automaton
    make_automaton(const Ctx&, AutIn&& aut)
    {
      return std::make_shared<AutOut>(std::move(aut));
    }
  }
}

#endif // !VCSN_DYN_AUTOMATON_HH
