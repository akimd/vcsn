#pragma once

#include <cstring> // strstr
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
      // We check validity by checking whether constant_term succeeds:
      // `1*` will raise.
      constant_term(rs, e);
      return true;
    }
    catch (const std::runtime_error& e)
    {
      // Some operators prevent the computation of the constant-term,
      // and raise an exception.  Forward that failure.
      if (std::strstr(e.what(), "not supported:"))
        raise("is_valid", strchr(e.what(), ':'));
      // Make sure this is really the failure we are looking for.
      if (!std::strstr(e.what(), "value is not starrable"))
        std::cerr << "warning: is_valid: unexpected error: "
                  << e.what() << '\n';
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
