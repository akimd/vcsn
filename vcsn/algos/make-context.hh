#pragma once

#include <sstream>

#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/expansion.hh>
#include <vcsn/misc/raise.hh>

namespace vcsn
{

  /*---------------.
  | make_context.  |
  `---------------*/

  template <typename Ctx>
  Ctx
  make_context(const std::string& name)
  {
    std::istringstream is{name};
    auto res = Ctx::make(is);
    // Something extremely weird is going wrong with str_escape when
    // called here from Python.  I have not been able to understand
    // what the problem was, and maybe it's actually a problem bw the
    // compiler (clang 3.4), the c++ lib (libstc++), and Python, and
    // possibly Boost after all.
    //
    // The good news is that this seems to work properly.
    if (is.peek() != -1)
      raise(__func__, ": invalid context name: ", str_escape(name),
            ", unexpected ", str_escape(is.peek()));
    return res;
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
        return dyn::make_context(vcsn::make_context<Ctx>(name));
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
      template <typename Aut>
      context
      context_of(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return dyn::make_context(a->context());
      }

      /// Bridge (context_of).
      template <typename ExpSet>
      context
      context_of_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>().expressionset();
        return dyn::make_context(e.context());
      }

      /// Bridge (context_of).
      template <typename ExpansionSet>
      context
      context_of_expansion(const expansion& exp)
      {
        const auto& e = exp->as<ExpansionSet>().expansionset();
        return dyn::make_context(e.context());
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
        return dyn::make_context(join(c1->as<Ctx1>(), c2->as<Ctx2>()));
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
        return ::vcsn::dyn::make_context(::vcsn::detail::make_word_context(c));
      }
    }
  }


  /*-------------.
  | num_tapes.   |
  `-------------*/

  template <typename Ctx>
  constexpr auto
  num_tapes(const Ctx&)
    -> std::enable_if_t<Ctx::is_lat, size_t>
  {
    return Ctx::labelset_t::size();
  }

  template <typename Ctx>
  constexpr auto
  num_tapes(const Ctx&)
    -> std::enable_if_t<!Ctx::is_lat, size_t>
  {
    return 0;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx>
      size_t
      num_tapes(const context& ctx)
      {
        return vcsn::num_tapes(ctx->as<Ctx>());
      }
    }
  }


} // vcsn::
