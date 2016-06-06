#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /// Expand a typed expression, i.e., distribute multiplications over
  /// sums.
  template <typename ExpSet>
  typename ExpSet::value_t
  expand(const ExpSet& rs, const typename ExpSet::value_t& r)
  {
    // FIXME: this does not work, but it should.
    // return ::vcsn::rat::copy(rs,
    //                          make_expressionset(rs, "distributive"),
    //                          r);
    return conv(make_expressionset(rs, "distributive"),
                to_string(rs, r));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet>
      expression
      expand(const expression& exp)
      {
        const auto& e = exp->as<ExpSet>();
        return {e.valueset(), ::vcsn::expand(e.valueset(), e.value())};
      }
    }
  }
}
