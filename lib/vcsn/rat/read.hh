#pragma once

#include <vcsn/core/rat/fwd.hh> // identities
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/export.hh>
#include <vcsn/misc/location.hh>

namespace vcsn
{
  namespace rat LIBVCSN_API
  {
    /// The expression in stream \a is.
    dyn::expression
    read(const dyn::context& ctx, rat::identities ids,
         std::istream& is,
         const location& l = location{});
  }
}
