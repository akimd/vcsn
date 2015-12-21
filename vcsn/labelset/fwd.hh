#pragma once

#include <vcsn/core/automaton.hh>
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{

  // letterset.hh.
  template <typename GenSet>
  class letterset;

  // nullableset.hh.
  template <typename LabelSet>
  class nullableset;

  // oneset.hh.
  class oneset;

  // stateset.hh.
  template <Automaton Aut>
  class stateset;

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
