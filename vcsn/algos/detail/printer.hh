#pragma once

#include <iostream>

#include <boost/range/algorithm/sort.hpp>

#include <vcsn/algos/sort.hh> // transition_less.
#include <vcsn/concepts/automaton.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn::detail
{
   /// Number of tapes.
   template <typename LabelSet>
   struct rank
   {
     static constexpr size_t value = 1;
   };

   template <typename... LabelSet>
   struct rank<tupleset<LabelSet...>>
   {
     static constexpr size_t value = tupleset<LabelSet...>::size();
   };

  /*-----------.
  | printer.   |
  `-----------*/

  /// \brief Factor common bits in automaton formatting.
  ///
  /// \tparam Aut an automaton type.
  template <Automaton Aut>
  class printer
  {
  protected:
    using automaton_t = Aut;

  public:
    printer(const automaton_t& aut, std::ostream& out)
      : aut_(aut)
      , os_(out)
    {}

    // Should not be public, but needed by GCC 4.8.1.
    // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=58972
    using state_t = state_t_of<automaton_t>;

  protected:
    using context_t = context_t_of<automaton_t>;
    using label_t = label_t_of<automaton_t>;
    using transition_t = transition_t_of<automaton_t>;
    using weightset_t = weightset_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;
    using polynomialset_t = polynomialset<context_t>;
    using polynomial_t = typename polynomialset_t::value_t;

    /// A list of states.
    using states_t = std::vector<state_t>;

    /// Convert a label to its representation.
    virtual std::string
    label_(const label_t& l) const
    {
      return ls_.is_one(l) ? "@epsilon" : to_string(ls_, l);
    }

    /// Output the transition \a t.  Do not insert eol.
    /// "Src Label Dst".
    virtual void print_transition_(transition_t t) const
    {
      aut_->print_state(aut_->src_of(t), os_);
      os_ << ' ' << label_(aut_->label_of(t)) << ' ';
      aut_->print_state(aut_->dst_of(t), os_);
    }

    /// Output transitions, sorted lexicographically on (Label, Dest).
    void print_state_(const state_t s)
    {
      std::vector<transition_t> ts;
      for (auto t : out(aut_, s))
        ts.emplace_back(t);
      boost::sort(ts, detail::transition_less<Aut>{aut_});
      for (auto t : ts)
        {
          os_ << '\n';
          print_transition_(t);
        }
    }

    /// Output transitions, sorted lexicographically.
    /// "Src Label Dst\n".
    void print_transitions_()
    {
      for (auto s: aut_->states())
        print_state_(s);
    }

    /// List names of states in \a ss, preceded by ' '.
    void list_states_(const states_t& ss)
    {
      for (auto s: ss)
        {
          os_ << ' ';
          aut_->print_state(s, os_);
        }
    }

    /// The list of initial states, sorted.
    states_t initials_()
    {
      states_t res;
      for (auto t: initial_transitions(aut_))
        res.emplace_back(aut_->dst_of(t));
      boost::sort(res);
      return res;
    }

    /// The list of final states, sorted.
    states_t finals_()
    {
      states_t res;
      for (auto t: final_transitions(aut_))
        res.emplace_back(aut_->src_of(t));
      boost::sort(res);
      return res;
    }

    /// The automaton we have to output.
    automaton_t aut_;
    /// Output stream.
    std::ostream& os_;
    /// Short-hand to the labelset.
    const labelset_t_of<automaton_t>& ls_ = *aut_->labelset();
    /// Short-hand to the weightset.
    const weightset_t& ws_ = *aut_->weightset();
    /// Short-hand to the polynomialset used to print the entries.
    const polynomialset_t ps_{aut_->context()};
    /// Whether is a transducer (two-tape automaton) as opposed to
    /// an acceptor.
    using is_transducer_t =
         std::integral_constant<bool,
                                2 <= rank<labelset_t_of<automaton_t>>::value>;
    const is_transducer_t is_transducer_ = {};
  };
}
