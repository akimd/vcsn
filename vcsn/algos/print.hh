#ifndef VCSN_ALGOS_PRINT_HH
# define VCSN_ALGOS_PRINT_HH

# include <iosfwd>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/weight.hh>
# include <vcsn/core/rat/ratexpset.hh>

namespace vcsn
{

  /*------------------------.
  | print(ratexp, stream).  |
  `------------------------*/

  template <typename Context>
  inline
  std::ostream&
  print(const Context& ctx, const rat::exp_t& e, std::ostream& o)
  {
    auto ratexpset = ctx.make_ratexpset();
    return ratexpset.print(o, ctx.downcast(e));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Abstract but parameterized.
      template <typename Context>
      std::ostream& print(const ratexp& exp, std::ostream& o)
      {
        const auto& ctx = dynamic_cast<const Context&>(exp->ctx());
        return vcsn::print<Context>(ctx, exp->ratexp(), o);
      }

      REGISTER_DECLARE(print_exp,
                       (const ratexp& aut, std::ostream& o) -> std::ostream&);
    }
  }

  /*------------------------.
  | print(weight, stream).  |
  `------------------------*/

  template <typename Context>
  inline
  std::ostream&
  print(const Context& ctx, const typename Context::weight_t& w,
        std::ostream& o)
  {
    return ctx.weightset()->print(o, w);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Abstract but parameterized.
      template <typename Context>
      std::ostream& print(const weight& w, std::ostream& o)
      {
        using weight_t = concrete_abstract_weight<Context>;
        const auto& typed_w = dynamic_cast<const weight_t&>(*w);
        return vcsn::print<Context>(typed_w.ctx(), typed_w.weight(), o);
      }

      REGISTER_DECLARE(print_weight,
                       (const weight& aut, std::ostream& o) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_PRINT_HH
