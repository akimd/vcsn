#ifndef VCSN_ALGOS_SHORTEST_HH
# define VCSN_ALGOS_SHORTEST_HH

# include <algorithm>
# include <iostream>
# include <list>
# include <map>
# include <queue>
# include <vector>

# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/military_order.hh>
# include <vcsn/weights/polynomialset.hh>

namespace vcsn
{

  /*----------------------.
  | shortest(automaton).  |
  `----------------------*/

  namespace detail
  {
    template <class Aut>
    class shortester
    {
    public:
      static_assert(Aut::context_t::is_lal,
                    "requires labels_are_letters");

      using automaton_t = Aut;
      using label_t = typename automaton_t::label_t;
      using state_t = typename automaton_t::state_t;
      using word_t = typename automaton_t::labelset_t::word_t;

      bool
      operator()(const automaton_t& aut, word_t& word)
      {
        const auto& ls = *aut.labelset();
        const auto& gs = *ls.genset();
        // Shortest word read at this state.
        using theword_t = std::map<state_t, word_t>;
        theword_t theword;

        // Breadth-first search.
        std::queue<state_t> thequeue;

        for (auto t: aut.initial_transitions())
          {
            state_t ini = aut.dst_of(t);
            theword[ini] = gs.empty_word();
            if (aut.is_final(ini))
              {
                word = theword[ini];
                return true;
              }
            else
              thequeue.push(ini);
          }

        while (!thequeue.empty())
          {
            state_t src = std::move(thequeue.front());
            thequeue.pop();
            for (const auto t: aut.out(src))
              {
                state_t dst = aut.dst_of(t);
                if (theword.find(dst) == theword.end())
                  {
                    theword[dst] = ls.concat(theword[src], aut.label_of(t));
                    if (aut.is_final(dst))
                      {
                        word = theword[dst];
                        return true;
                      }
                    else
                      thequeue.push(dst);
                  }
              }
          }
        return false;
      }
    };
  }

  template <typename Automaton>
  inline
  typename Automaton::labelset_t::word_t
  shortest(const Automaton& aut)
  {
    typename Automaton::labelset_t::word_t res;
    detail::shortester<Automaton> shortest;
    if (shortest(aut, res))
      return res;
    else
      throw std::domain_error("shortest: the automaton is useless");
  }


  /*----------------.
  | dyn::shortest.  |
  `----------------*/

  namespace dyn
  {
    namespace detail
    {

      // FIXME: We need dyn::label.
      template <typename Aut>
      std::string
      shortest(const automaton& aut)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        return a.labelset()->genset()->format(shortest(a));
      }

      REGISTER_DECLARE(shortest,
                       (const automaton& aut) -> std::string);
    }
  }


  /*-----------------------.
  | enumerate(automaton).  |
  `-----------------------*/

  namespace detail
  {
    template <typename Aut>
    class enumerater
    {
    public:
      using automaton_t = Aut;
      using context_t = typename Aut::context_t;
      static_assert(context_t::is_lal,
                    "requires labels_are_letters");

      using polynomialset_t = polynomialset<context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using label_t = typename automaton_t::label_t;
      using weight_t = typename automaton_t::weight_t;
      using state_t = typename automaton_t::state_t;
      using genset_t = typename automaton_t::labelset_t::genset_t;
      using word_t = typename genset_t::word_t;

      polynomial_t operator()(const automaton_t& aut, size_t max_length)
      {
        const auto& ls = *aut.labelset();
        const auto& ws = *aut.weightset();
        polynomialset_t ps(aut.context());

        // For each state, the first orders of its past.
        std::map<state_t, polynomial_t> past;

        using queue_t = std::list<std::tuple<state_t, word_t, weight_t>>;
        queue_t queue1, queue2;

        ps.set_weight(past[aut.pre()],
                      ls.genset()->empty_word(),
                      aut.weightset()->one());
        queue1.emplace_back(aut.pre(),
                            ls.genset()->empty_word(), aut.weightset()->one());

        // We match words that include the initial and final special
        // character.
        max_length += 2;
        for (size_t i = 0; i < max_length && not queue1.empty(); ++i)
          {
            while (!queue1.empty())
              {
                auto thepair = std::move(queue1.front());
                queue1.pop_front();
                state_t s = std::get<0>(thepair);
                word_t oldword = std::get<1>(thepair);
                weight_t oldweight = std::get<2>(thepair);
                for (const auto t: aut.all_out(s))
                  {
                    word_t newword = ls.concat(oldword, aut.label_of(t));
                    weight_t newweight = ws.mul(oldweight, aut.weight_of(t));
                    ps.add_weight(past[aut.dst_of(t)], newword, newweight);
                    queue2.emplace_back(aut.dst_of(t), newword, newweight);
                  }
              }
            queue1.swap(queue2);
          }

        // Return the past of post(), but remove the initial and final
        // special characters for the words.
        polynomial_t res;
        for (const auto& m: past[aut.post()])
          ps.add_weight(res,
                        ls.genset()->undelimit(m.first), m.second);
        return res;
      }
    };
  }

  template <typename Automaton>
  inline
  typename polynomialset<typename Automaton::context_t>::value_t
  enumerate(const Automaton& aut, size_t max)
  {
    detail::enumerater<Automaton> enumerate;
    return enumerate(aut, max);
  }


  /*-----------------.
  | dyn::enumerate.  |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {

      // FIXME: We need dyn::polynomial.
      template <typename Aut>
      std::vector<std::string>
      enumerate(const automaton& aut, size_t max)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        polynomialset<typename Aut::context_t> ps(a.context());
        std::vector<std::string> res;
        for (const auto& m: enumerate(a, max))
          res.emplace_back(ps.format(m));
        return res;
      }

      REGISTER_DECLARE
      (enumerate,
       (const automaton& aut, size_t max) -> std::vector<std::string>);
    }
  }
}

#endif // !VCSN_ALGOS_SHORTEST_HH
