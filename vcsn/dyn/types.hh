#pragma once

#include <vcsn/core/rat/identities.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn LIBVCSN_API
  {
    /// Direction for proper on automata.
    using ::vcsn::direction;

    /// Sets of identities on expressions.
    using ::vcsn::rat::identities;

    /// The type of a word.
    /// This is used by Tafkit to infer
    //  how to convert its arguments.
    using word = label;
  }
}
