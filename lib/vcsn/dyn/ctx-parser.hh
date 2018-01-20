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

  /// A context, normalized.
  ///
  /// E.g., 'lal(ab), b' -> 'letterset<char_letters(ab)>, b'.
  ///
  /// \param ctx   the context to normalize.
  /// \param full  whether to print the runtime values.
  ///    (e.g., the generators: 'letterset<char_letters(ab)>, b' or
  ///                           'letterset<char_letters>, b').
  std::string normalize_context(const std::string& ctx, bool full = true);
}
