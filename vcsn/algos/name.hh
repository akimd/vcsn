#pragma once

namespace vcsn
{
  /*--------------------.
  | name(expression).   |
  `--------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (name).
      template <typename ExpSet, typename String>
      expression
      name_expression(const expression& exp, const std::string& name)
      {
        const auto& e = exp->as<ExpSet>();
        return {e.valueset(),
                e.valueset().name(e.value(), symbol{name})};
      }
    }
  }
}
