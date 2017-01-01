#pragma once

#include <vcsn/core/rat/identities.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn LIBVCSN_API
  {
    /// Direction for proper on automata.
    using direction = ::vcsn::direction;

    /// Sets of identities on expressions.
    using identities = ::vcsn::rat::identities;

    /// The type of a word.
    /// Used by the Tools to infer how to convert such arguments.
    using word = label;
  }
}
