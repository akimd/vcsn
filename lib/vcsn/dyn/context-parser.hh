#pragma once

#include <memory>
#include <string>

#include <lib/vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace ast
  {
    /// Parse a user friendly context, and return its AST.
    std::shared_ptr<ast_node> parse_context(const std::string& ctx);
  }
}
