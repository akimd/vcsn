#pragma once

#include <vcsn/misc/symbol.hh>

namespace vcsn
{
  /// Request the most appropriate version of an algorithm.
  ///
  /// Used for determinization, and left-mult.
  struct auto_tag {};

  /// Request the Boolean specialization for determinization (B and
  /// F2).
  struct boolean_tag
  {
    static symbol sname()
    {
      static auto res = symbol{"boolean_tag"};
      return res;
    }
  };

  /// Dijkstra implementation.
  ///
  /// Used for lightest_path and scc.
  struct dijkstra_tag {};

  /// Request for the weighted version of an algorithm.
  ///
  /// Used for determinization, and minimization.
  struct weighted_tag
  {
    static symbol sname()
    {
      static auto res = symbol{"weighted_tag"};
      return res;
    }
  };
}
