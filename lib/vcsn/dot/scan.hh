#pragma once

// Set parameters for Flex header, and include it.
#define YY_FLEX_NAMESPACE_BEGIN namespace vcsn::detail::dot {
#define YY_FLEX_NAMESPACE_END   }

#include <vcsn/misc/location.hh>

YY_FLEX_NAMESPACE_BEGIN
  using location = vcsn::rat::location;
YY_FLEX_NAMESPACE_END

#include <lib/vcsn/dot/scan-version.hh>

#include <lib/vcsn/misc/flex-lexer.hh>
