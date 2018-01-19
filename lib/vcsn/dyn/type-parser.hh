#pragma once

#include <memory>
#include <string>

#include <lib/vcsn/dyn/fwd.hh>

namespace vcsn::ast
{
  /// Parse a C++ type, and return its AST.
  std::shared_ptr<ast_node> parse_type(const std::string& type);
}
