#pragma once

#include <memory>
#include <string>

#include <lib/vcsn/dyn/fwd.hh>

namespace vcsn::ast
{
  /// A context, translated.
  ///
  /// E.g., '[ab] -> b' -> 'letterset<char_letters(ab)>, b'.
  std::string translate_context(const std::string& ctx);
}
