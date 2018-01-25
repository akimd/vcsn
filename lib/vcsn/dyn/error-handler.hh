#pragma once

#include <map>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

#include <lib/vcsn/dyn/error-reporting.hh>

namespace vcsn::dyn::parser
{
  namespace x3 = boost::spirit::x3;

  /*---------------------.
  | Our error handler.   |
  `---------------------*/

  struct error_handler_base
  {
    error_handler_base()
    {
      id_map["ConText"] = "context";
    }

    template <typename Iterator, typename Exception, typename Context>
    x3::error_handler_result
    on_error(Iterator& /* first */, Iterator const&  /* last */,
             Exception const& x, Context const& context)
    {
      std::string which = x.which();
      auto iter = id_map.find(which);
      if (iter != id_map.end())
	which = iter->second;

      const auto message = "error: expecting " + which + " here:";
      auto& error_handler = x3::get<error_handler_tag>(context).get();
      error_handler(x.where(), message);
      return x3::error_handler_result::fail;
    }

    std::map<std::string, std::string> id_map;
  };
}
