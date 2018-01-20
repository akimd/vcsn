#pragma once

#include <boost/spirit/home/x3.hpp>

#include <lib/vcsn/dyn/error-handler.hh>

// config.hh
namespace vcsn::dyn::parser
{
  // Our Iterator Type
  using iterator_type = std::string::const_iterator;

  // The Phrase Parse context.
  using phrase_context_type
  = x3::phrase_parse_context<x3::ascii::space_type>::type;

  // Our Error Handler
  using error_handler_type = error_handler<iterator_type>;

  // Combined Error Handler and Phrase Parse context.
  using parse_context_type = x3::context<
      error_handler_tag
    , std::reference_wrapper<error_handler_type> const
    , phrase_context_type>;
}
