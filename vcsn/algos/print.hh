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
      std::ostream& print(const weight& weight, std::ostream& o)
      {
        const auto& w = weight->as<WeightSet>();
        return vcsn::print<WeightSet>(w.get_weightset(), w.weight(), o);
      }

      REGISTER_DECLARE(print_weight,
                       (const weight& aut, std::ostream& o) -> std::ostream&);
    }
  }

  /*------------------------.
  | print(ratexp, stream).  |
  `------------------------*/

#if 0
  // There is no need for this implementation, as the previous one,
  // for weightset/weight, applies here.  Actually, it results in an
  // ambiguous template specialization.
  template <typename RatExpSet>
  inline
  std::ostream&
  print(const RatExpSet& rs, const typename RatExpSet::ratexp_t& e,
        std::ostream& o)
  {
    return rs.print(o, e);
  }
#endif

  namespace dyn
  {
    namespace detail
    {
      /// Abstract but parameterized.
      template <typename RatExpSet>
      std::ostream& print(const ratexp& exp, std::ostream& o)
      {
        const auto& e = exp->as<RatExpSet>();
        return vcsn::print(e.get_ratexpset(), e.ratexp(), o);
      }

      REGISTER_DECLARE(print_exp,
                       (const ratexp& aut, std::ostream& o) -> std::ostream&);
    }
  }

}

#endif // !VCSN_ALGOS_PRINT_HH
