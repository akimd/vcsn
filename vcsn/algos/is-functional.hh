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
# include <vcsn/misc/unordered_set.hh>

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

    /// Words of the two-tape automaton: pairs of words.
    using wordset_t = detail::law_t<labelset_t>;
    using word_t = typename wordset_t::value_t;
    wordset_t ls = make_wordset(*aut->labelset());
    /// Common labelset for both tapes.
    auto ls1 = ls.template set<0>();

    // Input/Output path of a state.
    std::unordered_map<state_t, word_t> ios;

    auto coaccessibles = coaccessible_states(aut);
    std::unordered_set<state_t> marked;
    std::queue<state_t> todo;
    auto pre = aut->pre();
    todo.push(pre);
    marked.emplace(pre);
    ios.emplace(pre, ls.one());

    while (!todo.empty())
      {
        auto s = todo.front();
        todo.pop();
        for (auto t : aut->out(s))
          {
            auto l = aut->label_of(t);
            auto dst = aut->dst_of(t);
            if (has(coaccessibles, dst))
              {
                auto p = ios[s];
                p = ls.concat(p, l);
                auto c = ls1.lgcd(std::get<0>(p), std::get<1>(p));
                std::get<0>(p) = ls1.ldiv(c, std::get<0>(p));
                std::get<1>(p) = ls1.ldiv(c, std::get<1>(p));
                if (!has(ios, dst))
                  ios.emplace(dst, p);
                else if (!ls.equals(p, ios[dst]))
                  return false;
                if (aut->is_final(dst) && !ls.is_one(ios[dst]))
                  return false;
              }
            if (!has(marked, dst))
              {
                marked.emplace(dst);
                todo.emplace(dst);
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
    auto l = sort(blind<0>(aut))->strip();
    auto r = sort(insplit(blind<0>(aut)))->strip();
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
