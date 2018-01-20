#pragma once

#include <ostream>

#include <lib/vcsn/dyn/type-ast.hh>

namespace vcsn::ast
{
  /// Print a type ast in a "user friendly" manner.
  ///
  /// This is also the format which is used to name the generated
  /// files.
  ///
  /// \param t     the type ast
  /// \param full  whether to print the runtime values.
  ///    (e.g., the generators: 'letterset<char_letters(ab)>, b' or
  ///                           'letterset<char_letters>, b').
  std::string pretty(const std::shared_ptr<ast_node>& t, bool full = true);
}
