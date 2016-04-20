#pragma once

#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*--------------------------.
  | identities(expression).   |
  `--------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      rat::identities
      identities(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        auto rs = e.valueset();
        return rs.identities();
      }
    }
  }
}
