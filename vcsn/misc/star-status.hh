#pragma once

namespace vcsn
{
  enum star_status_t
    {
      /// valid iff proper succeeds on the "absolute value" of
      /// the automaton
      ABSVAL,

      // valid iff there is no epsilon-circuit with weight zero. Warning: the
      // property tested here is the acyclicity, which is equivalent only in
      // zero divisor free semirings.
      NON_STARRABLE,

      /// always valid.
      STARRABLE,

      /// valid iff proper succeeds.
      TOPS,
    };
};
