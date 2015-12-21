#pragma once

#include <memory> // shared_ptr
#include <string>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {
      /// Base class for automata.
      class LIBVCSN_API automaton_base
      {
      public:
        /// A description of the automaton, sufficient to build it.
        virtual symbol vname() const = 0;

        /// Extract wrapped typed automaton.
        template <Automaton Aut>
        auto& as()
        {
          return dyn_cast<automaton_wrapper<Aut>&>(*this).automaton();
        }

        /// Extract wrapped typed automaton.
        template <Automaton Aut>
        auto& as() const
        {
          return dyn_cast<const automaton_wrapper<Aut>&>(*this).automaton();
        }
      };

      /// A wrapped typed automaton.
      template <Automaton Aut>
      class automaton_wrapper final: public automaton_base
      {
      public:
        using automaton_t = Aut;

        automaton_wrapper(automaton_t aut)
          : automaton_(aut)
        {}

        virtual symbol vname() const override
        {
          return automaton()->sname();
        }

        automaton_t& automaton()
        {
          return automaton_;
        }

        const automaton_t& automaton() const
        {
          return automaton_;
        }

      private:
        /// The automaton.
        automaton_t automaton_;
      };
    }

    using automaton = std::shared_ptr<detail::automaton_base>;

    /// Build a dyn::automaton.
    template <Automaton Aut>
    inline
    automaton
    make_automaton(const Aut& aut)
    {
      return std::make_shared<detail::automaton_wrapper<Aut>>(aut);
    }
  }
}
