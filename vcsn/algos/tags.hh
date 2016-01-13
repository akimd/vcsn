#pragma once

namespace vcsn
{
  /// Request the most appropriate version of an algorithm.
  ///
  /// Used for determinization, and left-mult.
  struct auto_tag {};

  /// Dijkstra implementation.
  ///
  /// Used for lightest_path and scc.
  struct dijkstra_tag {};

  /// Request for the weighted version of an algorithm.
  ///
  /// Used for determinization, and minimization.
  struct weighted_tag {};
}
