#pragma once

#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/location.hh>

namespace vcsn::detail
{
  namespace dot LIBVCSN_API
  {
    using location = vcsn::rat::location;

    /// The automaton in dot in stream \a is.
    ///
    /// \param is     the stream
    /// \param l      the initial location
    dyn::automaton
    read(std::istream& is, const location& l = location{});
  }
}
