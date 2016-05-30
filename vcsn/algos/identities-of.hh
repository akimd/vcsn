#pragma once

#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*-----------------------------.
  | identities_of(expression).   |
  `-----------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      rat::identities
      identities_of(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        auto rs = e.valueset();
        return rs.identities();
      }
    }
  }
}
