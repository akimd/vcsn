#pragma once

#include <algorithm> // move
#include <memory> // shared_ptr

#include <vcsn/concepts/automaton.hh> // Automaton
#include <vcsn/dyn/cast.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  namespace dyn
  {
    /// A dyn automaton.
    class LIBVCSN_API automaton
    {
    public:
      automaton(std::nullptr_t ptr)
        : self_(ptr)
      {}

      template <typename AutImpl>
      automaton(const std::shared_ptr<AutImpl>& aut)
        : self_(std::make_shared<model<std::shared_ptr<AutImpl>>>(aut))
      {}

      /// A description of the automaton, sufficient to build it.
      symbol vname() const
      {
        return self_->vname();
      }

      /// Extract wrapped typed automaton.
      template <Automaton Aut>
      auto& as()
      {
        return detail::dyn_cast<model<Aut>&>(*self_).automaton();
      }

      /// Extract wrapped typed automaton.
      template <Automaton Aut>
      const auto& as() const
      {
        return detail::dyn_cast<const model<Aut>&>(*self_).automaton();
      }

      auto* operator->()
      {
        return this;
      }

      const auto* operator->() const
      {
        return this;
      }

      bool operator!() const
      {
        return !self_;
      }

    private:
      /// Abstract wrapped typed automaton.
      struct base
      {
        virtual ~base() = default;
        virtual symbol vname() const = 0;
      };

      /// A wrapped typed automaton.
      template <Automaton Aut>
      struct model final : base
      {
        using automaton_t = Aut;

        model(automaton_t aut)
          : automaton_(std::move(aut))
        {}

        symbol vname() const
        {
          return automaton()->sname();
        }

        auto& automaton()
        {
          return automaton_;
        }

        const auto& automaton() const
        {
          return automaton_;
        }

        /// The automaton.
        automaton_t automaton_;
      };

      /// The wrapped automaton.
      std::shared_ptr<base> self_;
    };
  }
}
