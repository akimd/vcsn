#ifndef VCSN_ALGOS_IS_VALID_RATEXP_HH
# define VCSN_ALGOS_IS_VALID_RATEXP_HH

# include <stdexcept>

# include <vcsn/algos/constant-term.hh>

namespace vcsn
{

  /*-------------------.
  | is_valid(ratexp).  |
  `-------------------*/

  /// Whether \a e has only computable stars.
  template <typename RatExpSet>
  bool
  is_valid(const RatExpSet& rs, const typename RatExpSet::value_t& e)
  {
    try
    {
      constant_term(rs, e);
      return true;
    }
    catch (const std::runtime_error&)
    {
      return false;
    }
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet>
      bool
      is_valid_ratexp(const ratexp& exp)
      {
        const auto& e = exp->as<RatExpSet>();
        return ::vcsn::is_valid(e.ratexpset(), e.ratexp());
      }

      REGISTER_DECLARE(is_valid_ratexp, (const ratexp& e) -> bool);
    }
  }
} // namespace vcsn

#endif // !VCSN_ALGOS_IS_VALID_RATEXP_HH
