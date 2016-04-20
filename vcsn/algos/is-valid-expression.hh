#pragma once

#include <stdexcept>

#include <vcsn/algos/constant-term.hh>

namespace vcsn
{

  /*------------------------.
  | is_valid(expression).   |
  `------------------------*/

  /// Whether \a e has only computable stars.
  template <typename ExpSet>
  bool
  is_valid(const ExpSet& rs, const typename ExpSet::value_t& e)
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
      /// Bridge (is_valid).
      template <typename ExpSet>
      bool
      is_valid_expression(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return ::vcsn::is_valid(e.valueset(), e.value());
      }
    }
  }
} // namespace vcsn
