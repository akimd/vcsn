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
    ///
    /// \param ctx    the context of the expression
    /// \param ids    the identities of the expression
    /// \param is     the stream
    /// \param format the syntax: "ere", "vcsn", "default"...
    /// \param l      the initial location
    dyn::expression
    read(const dyn::context& ctx, identities ids,
         std::istream& is,
         const std::string& format = "default",
         const location& l = location{});
  }
}
