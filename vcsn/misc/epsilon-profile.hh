#pragma once

#include <ostream>
#include <tuple>

namespace vcsn
{
  namespace detail
  {
    /// This is used by some epsilon removal algorithms.

    /// The structure contains the data needed to compare the elimination order
    /// between states.
    /// Code profiling shows that we spend too much time iterating
    /// on outgoing and inconing transitions of states to compute
    /// the order of elimination.  This structure caches what needs
    /// to be compared, and provides the comparison operator.
    template <typename State>
    struct epsilon_profile
    {
      using state_t = State;

      /// From the heap's top, recover state to eliminate.
      state_t state;
      /// Number of incoming spontaneous transitions.
      size_t in_sp;
      /// Number of incoming non-spontaneous transitions.
      size_t in_nsp;
      /// Number of outgoing spontaneous transitions.
      size_t out_sp;
      /// Number of outgoing non-spontaneous transitions.
      size_t out_nsp;

      /// \brief Generate a state profile.
      ///
      /// \param s  state handle
      /// \param insp  number of incoming spontaneous transitions to \a s
      /// \param in    number of incoming transitions to \a a
      /// \param outsp number of outgoing spontaneous transitions from \a s
      /// \param out   number of outgoing transitions from \a s
      epsilon_profile(state_t s,
                      size_t insp, size_t in,
                      size_t outsp, size_t out)
        : state(s)
        , in_sp(insp), in_nsp(in - insp)
        , out_sp(outsp), out_nsp(out - outsp)
      {}

      void update(size_t insp, size_t in,
                  size_t outsp, size_t out)
      {
        in_sp = insp;
        in_nsp = in - insp;
        out_sp = outsp;
        out_nsp = out - outsp;
      }

      /// Whether l < r for the min-heap.
      ///
      /// Compare priorities: return true if \a l should be
      /// treated before \a r.  Must be strict.
      bool operator<(const epsilon_profile& r) const
      {
        // (i) First, work on those with fewer outgoing spontaneous
        // transitions.
        // (ii) Prefer fewer outgoing transitions.
        // (iii) Prefer fewer incoming spontaneous transitions.
        // (iv) Then, ensure total order using state handle.
        return (std::tie  (out_sp,   out_nsp,   in_sp,   r.state)
                < std::tie(r.out_sp, r.out_nsp, r.in_sp, state));
      }

      friend std::ostream& operator<<(std::ostream& o, const epsilon_profile& p)
      {
        return o << p.state
                 << 'o' << p.out_sp
                 << 'O' << p.out_nsp
                 << 'i' << p.in_sp
                 << 'I' << p.in_nsp;
      }
    };
  }
}
