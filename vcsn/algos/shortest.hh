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

      using labelset_t = typename automaton_t::labelset_t;
      using weightset_t = typename automaton_t::weightset_t;
      using polynomialset_t = polynomialset<context_t>;
      using polynomial_t = typename polynomialset_t::value_t;
      using label_t = typename automaton_t::label_t;
      using weight_t = typename automaton_t::weight_t;
      using state_t = typename automaton_t::state_t;
      using genset_t = typename automaton_t::labelset_t::genset_t;
      using word_t = typename genset_t::word_t;

      using queue_t = std::list<std::tuple<state_t, word_t, weight_t>>;

      enumerater(const automaton_t& aut)
        : aut_(aut)
      {
        ps_.set_weight(past_[aut_.pre()],
                       ls_.genset()->empty_word(),
                       aut_.weightset()->one());
      }

      polynomial_t operator()(size_t max_length)
      {
        queue_t queue;

        queue.emplace_back(aut_.pre(),
                           ls_.genset()->empty_word(),
                           aut_.weightset()->one());

        // We match words that include the initial and final special
        // character.
        max_length += 2;
        for (size_t i = 0; i < max_length && not queue.empty(); ++i)
          propagate_(queue);

        // Return the past of post(), but remove the initial and final
        // special characters for the words.
        polynomial_t res;
        for (const auto& m: past_[aut_.post()])
          ps_.add_weight(res,
                         ls_.genset()->undelimit(m.first), m.second);
        return res;
      }

    private:
      /// Process once all the states of \a q1.
      /// Save into q1 the new states to visit.
      void propagate_(queue_t& q1)
      {
        queue_t q2;
        while (!q1.empty())
          {
            auto thepair = std::move(q1.front());
            q1.pop_front();
            state_t s = std::get<0>(thepair);
            word_t oldword = std::get<1>(thepair);
            weight_t oldweight = std::get<2>(thepair);
            for (const auto t: aut_.all_out(s))
              {
                word_t newword = ls_.concat(oldword, aut_.label_of(t));
                weight_t newweight = ws_.mul(oldweight, aut_.weight_of(t));
                ps_.add_weight(past_[aut_.dst_of(t)], newword, newweight);
                q2.emplace_back(aut_.dst_of(t), newword, newweight);
              }
          }
        q1.swap(q2);
      }

      const automaton_t& aut_;
      const labelset_t& ls_ = *aut_.labelset();
      const weightset_t& ws_ = *aut_.weightset();
      const polynomialset_t ps_ = {aut_.context()};
      /// For each state, the first orders of its past.
      std::map<state_t, polynomial_t> past_;
    };
  }

  template <typename Automaton>
  inline
  typename polynomialset<typename Automaton::context_t>::value_t
  enumerate(const Automaton& aut, size_t max)
  {
    detail::enumerater<Automaton> enumerate(aut);
    return enumerate(max);
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
