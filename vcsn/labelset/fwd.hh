#pragma once

#include <vcsn/weightset/weightset.hh> // weightset_mixin.

namespace vcsn
{

  // letterset.hh.
  template <typename GenSet>
  class letterset;

  // oneset.hh.
  class oneset;

  // tupleset.hh.
  namespace detail
  {
    template <typename... LabelSets>
    class tupleset_impl;
  }

  template <typename... LabelSets>
  using tupleset = weightset_mixin<detail::tupleset_impl<LabelSets...>>;

  // wordset.hh.
  template <typename GenSet>
  class wordset;

} // namespace vcsn
