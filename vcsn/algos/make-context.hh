#pragma once

#include <sstream>

#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{
  /*---------------.
  | make_context.  |
  `---------------*/

  /// Build a context from its name.
  template <typename Ctx>
  Ctx
  make_context(const std::string& name)
  {
    std::istringstream is{name};
    try
      {
        auto res = Ctx::make(is);
        require(is.peek() == EOF, "unexpected trailing characters: ", is);
        return res;
      }
    catch (const std::runtime_error& e)
      {
        raise(e, "  while reading context: ", name);
      }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx>
      context
      make_context(const std::string& name)
      {
        return vcsn::make_context<Ctx>(name);
      }
    }
  }


  /*-------------.
  | context_of.  |
  `-------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      context
      context_of(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return context(a->context());
      }

      /// Bridge (context_of).
      template <typename ExpSet>
      context
      context_of_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>().valueset();
        return e.context();
      }

      /// Bridge (context_of).
      template <typename ExpansionSet>
      context
      context_of_expansion(const expansion& exp)
      {
        const auto& e = exp->as<ExpansionSet>().valueset();
        return e.context();
      }

      /// Bridge (context_of).
      template <typename PolynomialSet>
      context
      context_of_polynomial(const polynomial& poly)
      {
        const auto& p = poly->as<PolynomialSet>().valueset();
        return p.context();
      }
    }
  }


  /*--------.
  | join.   |
  `--------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx1, typename Ctx2>
      context
      join(const context& c1, const context& c2)
      {
        return join(c1->as<Ctx1>(), c2->as<Ctx2>());
      }
    }
  }



  /*--------------------.
  | make_word_context.  |
  `--------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx>
      context
      make_word_context(const context& ctx)
      {
        const auto& c = ctx->as<Ctx>();
        return ::vcsn::detail::make_word_context(c);
      }
    }
  }
}
