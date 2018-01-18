#pragma once

#include <stdexcept>
#include <string>

#include <vcsn/misc/fwd.hh>

namespace vcsn::dyn
{
  /// An exception suited for our compilation errors.
  struct jit_error: std::runtime_error
  {
    jit_error(const std::string& assert, const std::string& what);
    /// If defined, static assertions that failed (ends with a eol).
    std::string assertions;
  };

  /// Compile, and load, a DSO with instantiations for \a ctx.
  void compile(const std::string& ctx);

  /// Compile, and load, a DSO which instantiates \a algo for \a sig.
  void compile(const std::string& algo, const signature& sig);
}
