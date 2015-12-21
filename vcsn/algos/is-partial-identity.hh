#pragma once

#include <queue>
#include <utility>

#include <vcsn/algos/accessible.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/unordered_map.hh>

namespace vcsn
{

  /// Whether transducer \a aut is equivalent to a partial identity
  /// function on all successful paths.
  ///
  /// This automaton is expected to be two-tape, both tapes having the
  /// same labelset.
  template <Automaton Aut>
  bool is_partial_identity(const Aut& aut)
  {
    using state_t = state_t_of<Aut>;
    using labelset_t = labelset_t_of<Aut>;

    /// Words of the k-tape automaton: k-tuples of words.
    using wordset_t = detail::law_t<labelset_t>;
    using word_t = typename wordset_t::value_t;
    wordset_t ls = make_wordset(*aut->labelset());
    /// Common labelset for both tapes.
    auto ls1 = ls.template set<0>();
    /// Residue of input and output paths arriving to a given state,
    /// with longest common prefix eliminated.
    std::unordered_map<state_t, word_t> rs;

    auto coaccessibles = coaccessible_states(aut);
    std::queue<state_t> todo;
    auto pre = aut->pre();
    todo.push(pre);
    rs.emplace(pre, ls.one());
    // When reaching the final state, there must be no residue.
    // Instead of checking this case especially, just make sure
    // that when we reach post, we only collected (\e, \e).
    rs.emplace(aut->post(), ls.one());

    while (!todo.empty())
      {
        auto s = todo.front();
        todo.pop();
        for (auto t : aut->all_out(s))
          {
            auto dst = aut->dst_of(t);
            if (has(coaccessibles, dst))
              {
                // Compute the new residue.
                auto r = ls.mul(rs[s], aut->label_of(t));
                // Eliminate longest common prefix.
                ls.lnormalize_here(r);
                if (!has(rs, dst))
                  {
                    rs.emplace(dst, r);
                    todo.emplace(dst);
                  }
                else if (!ls.equal(r, rs[dst]))
                  {
                    return false;
                  }
              }
          }
      }
    return true;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_partial_identity(const automaton& aut)
      {
        return is_partial_identity(aut->as<Aut>());
      }
    }
  }
}
