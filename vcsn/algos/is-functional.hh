#ifndef VCSN_ALGOS_IS_FUNCTIONAL_HH
# define VCSN_ALGOS_IS_FUNCTIONAL_HH

# include <queue>
# include <utility>

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/compose.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/labelset/tupleset.hh>
# include <vcsn/misc/unordered_map.hh>

namespace vcsn
{
  /*---------------.
  | is-functional. |
  `---------------*/

  /// Whether transducer \a aut is equivalent to the identity function
  /// on all successful path.
  ///
  /// This automaton is expected to be two-tape, both tapes having the
  /// same labelset.
  template <typename Aut>
  bool is_identity(const Aut& aut)
  {
    using state_t = state_t_of<Aut>;
    using labelset_t = labelset_t_of<Aut>;

    /// Words of the k-tape automaton: k-tuples of words.
    using wordset_t = detail::law_t<labelset_t>;
    using word_t = typename wordset_t::value_t;
    wordset_t ls = make_wordset(*aut->labelset());
    /// Common labelset for both tapes.
    auto ls1 = ls.template set<0>();
    /// Residue of input and output path of states by eliminate
    /// longest common prefix.
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
            auto l = aut->label_of(t);
            auto dst = aut->dst_of(t);
            if (has(coaccessibles, dst))
              {
                auto p = rs[s];
                p = ls.concat(p, l);
                // Eliminate longest common prefix.
                ls.lnormalize_here(p);
                if (!has(rs, dst))
                  {
                    rs.emplace(dst, p);
                    todo.emplace(dst);
                  }
                else if (!ls.equals(p, rs[dst]))
                  return false;
              }
          }
      }
    return true;
  }

  /// Whether \a aut is functional.
  /// \pre \a aut is a transducer.
  template <typename Aut>
  bool is_functional(const Aut& aut)
  {
    // Compose aut and its invert.
    auto l = focus<0>(aut);
    auto r = insplit(focus<0>(aut));
    detail::composer<decltype(l), decltype(r)> compose(l, r);
    auto c = compose.compose();
    return is_identity(c);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <class Aut>
      bool is_functional(const automaton& aut)
      {
        return is_functional(aut->as<Aut>());
      }

      REGISTER_DECLARE(is_functional,
                       (const automaton&) -> bool);
    }
  }
}

#endif // !VCSN_ALGOS_IS_FUNCTIONAL_HH
