#ifndef VCSN_ALGOS_PRINT_HH
# define VCSN_ALGOS_PRINT_HH

# include <iosfwd>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/core/rat/ratexpset.hh>

namespace vcsn
{

  /*------------------------.
  | print(ratexp, stream).  |
  `------------------------*/

  template <class Context>
  inline
  std::ostream&
  print(const Context& ctx, const rat::exp_t& e, std::ostream& o)
  {
    auto ratexpset = ctx.make_ratexpset();
    return ratexpset.print(o, ctx.downcast(e));
  }

  namespace dyn
  {
    namespace details
    {
      /// Abstract but parameterized.
      template <typename Context>
      std::ostream& print(const dyn::ratexp& exp, std::ostream& o)
      {
        const auto& ctx = dynamic_cast<const Context&>(exp->ctx());
        return vcsn::print<Context>(ctx, exp->ratexp(), o);
      }

      using print_exp_t =
        auto (const ratexp& aut, std::ostream& o) -> std::ostream&;

      bool print_exp_register(const std::string& ctx, const print_exp_t& fn);
    }
  }
}

#endif // !VCSN_ALGOS_PRINT_HH
