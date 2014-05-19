#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <sstream>

# include <vcsn/algos/enumerate.hh> // make_word_context
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/dyn/ratexpset.hh>
# include <vcsn/misc/raise.hh>

namespace vcsn
{

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
      template <typename Aut>
      context
      context_of(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_context(a.context());
      }

      REGISTER_DECLARE(context_of,
                       (const automaton& aut) -> context);

      /// Bridge.
      template <typename RatExpSet>
      context
      context_of_ratexp(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>().ratexpset();
        return make_context(e.context());
      }

      REGISTER_DECLARE(context_of_ratexp,
                       (const ratexp& exp) -> context);

      /// Bridge.
      template <typename Ctx>
      context
      make_context(const std::string& name)
      {
        return dyn::make_context(vcsn::make_context<Ctx>(name));
      }

      REGISTER_DECLARE(make_context,
                       (const std::string& name) -> context);

      /*-----------------.
      | make_ratexpset.  |
      `-----------------*/

      template <typename Ctx>
      ratexpset
      make_ratexpset(const context& ctx)
      {
        const auto& c = ctx->as<Ctx>();
        return ::vcsn::dyn::make_ratexpset(::vcsn::ratexpset<Ctx>(c));
      }

      REGISTER_DECLARE(make_ratexpset,
                       (const context& ctx) -> ratexpset);

      /*--------------------.
      | make_word_context.  |
      `--------------------*/

      template <typename Ctx>
      context
      make_word_context(const context& ctx)
      {
        const auto& c = ctx->as<Ctx>();
        return ::vcsn::dyn::make_context(::vcsn::detail::make_word_context(c));
      }

      REGISTER_DECLARE(make_word_context,
                       (const context& ctx) -> context);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_MAKE_CONTEXT_HH
