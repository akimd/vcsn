#ifndef VCSN_ALGOS_IS_AMBIGUOUS_HH
# define VCSN_ALGOS_IS_AMBIGUOUS_HH

# include <vcsn/algos/accessible.hh>
# include <vcsn/algos/distance.hh>
# include <vcsn/algos/product.hh>
# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*---------------.
  | is_ambiguous.  |
  `---------------*/

  /// Whether an automaton is ambiguous.
  ///
  /// \param[in] aut        the automaton
  /// \param[out] witness   if ambiguous, a pair of "ambiguous" states.
  /// \returns whether ambiguous.
  template <typename Aut>
  bool is_ambiguous(const Aut& aut,
                    std::tuple<state_t_of<Aut>, state_t_of<Aut>>& witness)
  {
    auto prod = product(aut, aut);
    // Check if there useful states outside of the diagonal.  Since
    // the product is accessible, check only for coaccessibles states.
    auto coaccessible = coaccessible_states(prod);
    for (const auto& o: prod->origins())
      if (std::get<0>(o.second) != std::get<1>(o.second)
          && has(coaccessible, o.first))
        {
          witness = o.second;
          return true;
        }
    return false;
  }

  template <typename Aut>
  bool is_ambiguous(const Aut& aut)
  {
    std::tuple<state_t_of<Aut>, state_t_of<Aut>> dummy;
    return is_ambiguous(aut, dummy);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <class Aut>
      bool is_ambiguous(const automaton& aut)
      {
        return is_ambiguous(aut->as<Aut>());
      }

      REGISTER_DECLARE(is_ambiguous,
                       (const automaton&) -> bool);
    }
  }


  /*-----------------.
  | ambiguous_word.  |
  `-----------------*/

  template <typename Aut>
  typename labelset_t_of<Aut>::word_t ambiguous_word(const Aut& aut)
  {
    std::tuple<state_t_of<Aut>, state_t_of<Aut>> witness;
    require(is_ambiguous(aut, witness),
            "automaton is unambiguous");
    const auto& ls = *aut->labelset();
    typename labelset_t_of<Aut>::word_t res;
    // Find the shortest word from initial to the witness.
    auto s = std::get<0>(witness);
    for (auto t: path_bfs(aut, aut->pre(), s))
      {
        auto l = aut->label_of(t);
        if (!ls.is_special(l))
          res = ls.concat(res, l);
      }
    for (auto t: path_bfs(aut, s, aut->post()))
      {
        auto l = aut->label_of(t);
        if (!ls.is_special(l))
          res = ls.concat(res, l);
      }
    return res;
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      label
      ambiguous_word(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        auto word = vcsn::ambiguous_word(a);
        return make_label(make_wordset(*a->labelset()), word);
      }

      REGISTER_DECLARE(ambiguous_word,
                       (const automaton&) -> label);
    }
  }

}

#endif // !VCSN_ALGOS_IS_AMBIGUOUS_HH
