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

  template <typename RatExpSet>
  inline
  std::ostream&
  print(const RatExpSet& rs, const rat::exp_t& e, std::ostream& o)
  {
    const auto& ctx = rs.context();
    return rs.print(o, ctx.downcast(e));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Abstract but parameterized.
      template <typename Context>
      std::ostream& print(const ratexp& exp, std::ostream& o)
      {
        using ratexpset_t = vcsn::ratexpset<Context>;
        const auto& ctx = dynamic_cast<const Context&>(exp->ctx());
        auto rs = ratexpset_t(ctx);
        return vcsn::print<ratexpset_t>(rs, exp->ratexp(), o);
      }

      REGISTER_DECLARE(print_exp,
                       (const ratexp& aut, std::ostream& o) -> std::ostream&);
    }
  }

  /*------------------------.
  | print(weight, stream).  |
  `------------------------*/

  template <typename WeightSet>
  inline
  std::ostream&
  print(const WeightSet& ws, const typename WeightSet::value_t& w,
        std::ostream& o)
  {
    return ws.print(o, w);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Abstract but parameterized.
      template <typename WeightSet>
      std::ostream& print(const weight& w, std::ostream& o)
      {
        using weight_t = concrete_abstract_weight<WeightSet>;
        const auto& typed_w = dynamic_cast<const weight_t&>(*w);
        return vcsn::print<WeightSet>(typed_w.get_weightset(),
                                      typed_w.weight(), o);
      }

      REGISTER_DECLARE(print_weight,
                       (const weight& aut, std::ostream& o) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_PRINT_HH
