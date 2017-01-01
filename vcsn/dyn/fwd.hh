#pragma once

#include <vcsn/core/rat/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    // vcsn/dyn/automaton.hh.
    class automaton;

    // vcsn/dyn/context.hh.
    class context;

    // vcsn/dyn/types.hh.
    using identities = ::vcsn::rat::identities;

    // vcsn/dyn/value.hh.
    namespace detail
    {
      struct expansion_tag;
      struct expression_tag;
      struct label_tag;
      struct polynomial_tag;
      struct weight_tag;
    }
    template <typename tag>
    class value_impl;
    using expansion = value_impl<detail::expansion_tag>;
    using expression = value_impl<detail::expression_tag>;
    using label = value_impl<detail::label_tag>;
    using polynomial = value_impl<detail::polynomial_tag>;
    using weight = value_impl<detail::weight_tag>;
  }
}
