#pragma once

#include <iostream>

#include <boost/range/algorithm/sort.hpp>

#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/algos/sort.hh> // transition_less.
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/weightset/fwd.hh> // b
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  namespace detail
  {

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
    };

  }


  /*---------.
  | fadoer.  |
  `---------*/

  namespace detail
  {

    /// \brief Format an automaton into Fado.
    ///
    /// \tparam Aut an automaton type, not a pointer type.
    template <Automaton Aut>
    class fadoer: public printer<Aut>
    {
      static_assert(context_t_of<Aut>::is_lal || context_t_of<Aut>::is_lan,
                    "fado: requires letter or nullable labels");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "fado: requires Boolean weights");

      using super_t = printer<Aut>;

      using super_t::aut_;
      using super_t::finals_;
      using super_t::initials_;
      using super_t::list_states_;
      using super_t::os_;
      using super_t::print_transitions_;
      using super_t::ws_;

      using super_t::super_t;

    public:
      /// Actually output \a aut_ on \a os_.
      // http://www.dcc.fc.up.pt/~rvr/FAdoDoc/_modules/fa.html#saveToFile
      void operator()()
      {
        bool is_deter = is_deterministic_(aut_);
        os_ << (is_deter ? "@DFA" : "@NFA");
        list_states_(finals_());
        if (!is_deter)
          {
            os_ << " *";
            list_states_(initials_());
          }
        print_transitions_();
      }

    private:
      template <typename A>
      std::enable_if_t<labelset_t_of<A>::is_free(), bool>
      is_deterministic_(const A& a)
      {
        return vcsn::is_deterministic(a);
      }

      template <typename A>
      std::enable_if_t<!labelset_t_of<A>::is_free(), bool>
      is_deterministic_(const A&)
      {
        return false;
      }
    };

  }

  template <Automaton Aut>
  std::ostream&
  fado(const Aut& aut, std::ostream& out)
  {
    auto fado = detail::fadoer<Aut>{aut, out};
    fado();
    return out;
  }



  /*----------.
  | grailer.  |
  `----------*/

  namespace detail
  {
    /// \brief Format an automaton into Fado.
    ///
    /// \tparam Aut an automaton type, not a pointer type.
    ///
    /// See https://cs.uwaterloo.ca/research/tr/1993/01/93-01.pdf.
    template <Automaton Aut>
    class grailer: public printer<Aut>
    {
      static_assert(context_t_of<Aut>::is_lal || context_t_of<Aut>::is_lan,
                    "grail: requires letter or nullable labels");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "grail: requires Boolean weights");

      using super_t = printer<Aut>;

      using typename super_t::automaton_t;
      using typename super_t::state_t;
      using typename super_t::transition_t;

      using super_t::aut_;
      using super_t::finals_;
      using super_t::initials_;
      using super_t::os_;
      using super_t::print_transitions_;
      using super_t::ws_;

      using super_t::super_t;

    public:
      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        // Don't end with a \n.
        const char* sep = "";
        for (auto s: initials_())
          {
            os_ << sep
                << "(START) |- ";
            aut_->print_state(s, os_);
            sep = "\n";
          }
        print_transitions_();
        for (auto s: finals_())
          {
            os_ << '\n';
            aut_->print_state(s, os_) <<  " -| (FINAL)";
          }
      }
    };
  }

  template <Automaton Aut>
  std::ostream&
  grail(const Aut& aut, std::ostream& out)
  {
    auto grail = detail::grailer<Aut>{aut, out};
    grail();
    return out;
  }
}
