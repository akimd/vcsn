#pragma once

#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/value.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  /*------------------.
  | read_automaton.   |
  `------------------*/

  /// Static version of the read_automaton function.
  ///
  /// Read an automaton with a specified context (from the Aut
  /// template parameter).
  template <Automaton Aut>
  Aut
  read_automaton(std::istream& is, const std::string& format)
  {
    auto aut = dyn::read_automaton(is, format, true);
    // Automaton typename.
    auto vname = aut->vname();
    VCSN_REQUIRE(vname == Aut::element_type::sname(),
                 format, ": invalid context: ", vname,
                 ", expected: ", Aut::element_type::sname());
    return std::move(aut->as<Aut>());
  }


  /*-------------.
  | read_label.  |
  `-------------*/

  template <typename Context>
  auto
  read_label(const Context& ctx, std::istream& is,
             bool quoted = false)
    -> label_t_of<Context>
  {
    return ctx.labelset()->conv(is, quoted);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Istream, typename Bool>
      label
      read_label(const context& ctx, std::istream& is, bool quoted)
      {
        const auto& c = ctx->as<Context>();
        auto res = ::vcsn::read_label(c, is, quoted);
        return {*c.labelset(), res};
      }
    }
  }


  /*------------------.
  | read_polynomial.  |
  `------------------*/

  template <typename Context>
  auto
  read_polynomial(const Context& ctx, std::istream& is)
    -> typename polynomialset<Context>::value_t
  {
    return polynomialset<Context>(ctx).conv(is);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Istream>
      polynomial
      read_polynomial(const context& ctx, std::istream& is)
      {
        const auto& c = ctx->as<Context>();
        auto ps = polynomialset<Context>(c);
        auto res = ::vcsn::read_polynomial(c, is);
        return {ps, res};
      }
    }
  }


  /*--------------.
  | read_weight.  |
  `--------------*/

  template <typename Context>
  auto
  read_weight(const Context& ctx, std::istream& is)
    -> weight_t_of<Context>
  {
    return ctx.weightset()->conv(is);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Context, typename Istream>
      weight
      read_weight(const context& ctx, std::istream& is)
      {
        const auto& c = ctx->as<Context>();
        auto res = ::vcsn::read_weight(c, is);
        return {*c.weightset(), res};
      }
    }
  }
} // namespace vcsn
