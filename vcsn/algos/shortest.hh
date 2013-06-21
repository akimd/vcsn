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
            state_t i = std::move(thequeue.front());
            thequeue.pop();
            for (const auto a: ls)
              // Iterate over successors of i by a.
              for (const auto t: aut.out(i, a))
                {
                  state_t j = aut.dst_of(t);
                  std::cerr << "State: " << j << std::endl;
                  if (theword.find(j) == theword.end())
                    {
                      theword[j] = ls.concat(theword[i], a);
                      if (aut.is_final(j))
                        {
                          word = theword[j];
                          return true;
                        }
                      else
                        thequeue.push(j);
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
        return a.labelset()->format(shortest(a));
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
    template <typename Aut, typename Container>
    class enumerater
    {
    public:
      static_assert(Aut::context_t::is_lal,
                    "requires labels_are_letters");

      using automaton_t = Aut;
      using container_t = Container;
      using label_t = typename automaton_t::label_t;
      using state_t = typename automaton_t::state_t;
      using genset_t = typename automaton_t::labelset_t::genset_t;
      using word_t = typename genset_t::word_t;

      void operator()(const automaton_t& aut, size_t max_length,
                      Container& words)
      {
        // The LabelSet.
        const auto& ls = *aut.labelset();
        const auto& gs = *ls.genset();

        // a list of words that leads to this state
        std::map<state_t, container_t> theword;
        // std::list allows swap, contrary to std::queue.
        using queue_t = std::list<std::pair<state_t, word_t>>;
        queue_t queue1, queue2;

        for (auto t: aut.initial_transitions())
          {
            state_t ini = aut.dst_of(t);
            theword[ini].push_back(gs.empty_word());
            queue1.emplace_back(ini, gs.empty_word());
          }

        for (size_t i = 0; i < max_length && not queue1.empty(); ++i)
          {
            while (!queue1.empty())
              {
                std::pair<state_t, word_t> thepair = std::move(queue1.front());
                queue1.pop_front();
                state_t s = thepair.first;
                word_t oldword = thepair.second;
                for (const auto a: ls)
                  for (const auto t: aut.out(s, a))
                    {
                      word_t newword = ls.concat(oldword, a);
                      theword[aut.dst_of(t)].push_back(newword);
                      queue2.emplace_back(aut.dst_of(t), newword);
                    }
              }
            queue1.swap(queue2);
          }

        std::vector<word_t> v;
        for (const auto t: aut.final_transitions())
          {
            state_t j = aut.src_of(t);
            auto &l = theword[j];
            v.insert(v.end(), l.begin(), l.end());
          }
        sort(v.begin(), v.end(), MilitaryOrder<word_t>());
        auto v_last = std::unique(v.begin(), v.end());
        words.insert(words.begin(), v.begin(), v_last);
      }
    };
  }

  template <typename Automaton>
  inline
  std::vector<typename Automaton::labelset_t::word_t>
  enumerate(const Automaton& aut, size_t max)
  {
    using res_t = std::vector<typename Automaton::labelset_t::word_t>;
    res_t res;
    detail::enumerater<Automaton, res_t> enumerate;
    enumerate(aut, max, res);
    return std::move(res);
  }


  /*-----------------.
  | dyn::enumerate.  |
  `-----------------*/

  namespace dyn
  {
    namespace detail
    {

      // FIXME: We need dyn::label.
      template <typename Aut>
      std::vector<std::string>
      enumerate(const automaton& aut, size_t max)
      {
        const auto& a = dynamic_cast<const Aut&>(*aut);
        std::vector<std::string> res;
        for (const auto& r: enumerate(a, max))
          res.emplace_back(a.labelset()->format(r));
        return res;
      }

      REGISTER_DECLARE
      (enumerate,
       (const automaton& aut, size_t max) -> std::vector<std::string>);
    }
  }
}

#endif // ! VCSN_ALGOS_ENUMERATE_HH
