#pragma once

#include <memory>
#include <string>

#include <lib/vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace ast
  {
    /// Parse a context, and return its AST.
    std::shared_ptr<ast_node> parse_context(const std::string& ctx);

    /// Parse a type, and return its AST.
    std::shared_ptr<ast_node> parse_type(const std::string& type);
  }
}
