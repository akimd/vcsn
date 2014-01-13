#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <istream>

# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/ctx/fwd.hh>
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
    std::string remainder;
    is >> remainder;
    require(remainder.empty(),
            __func__, ": unexpected characters after context: ",
            str_escape(remainder));
    return res;
  }

  /*----------.
  | dynamic.  |
  `----------*/

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

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_MAKE_CONTEXT_HH
