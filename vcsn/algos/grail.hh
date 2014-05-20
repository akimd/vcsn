#ifndef VCSN_ALGOS_GRAIL_HH
# define VCSN_ALGOS_GRAIL_HH

# include <iostream>
# include <map>

# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  namespace detail
  {

    /*------------.
    | outputter.  |
    `------------*/

    /// \brief Factor common bits in automaton formatting.
    ///
    /// \tparam Aut an automaton type, not a pointer type.
    template <typename Aut>
    class outputter
    {
    protected:
      using automaton_t = Aut;

    public:
      outputter(const automaton_t& aut, std::ostream& out)
        : aut_(aut)
        , os_(out)
      {
        // Build a (now trivial) map from state to printed number.
        for (auto t: aut_.states())
          states_.emplace(t, t - 2);
      }

      // Should not be public, but needed by GCC 4.8.1.
      // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=58972
      using state_t = typename automaton_t::state_t;

    protected:
      using context_t = context_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;
      using transition_t = typename automaton_t::transition_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename automaton_t::weight_t;

      using states_t = std::vector<state_t>;

      /// Convert a label to its representation.
      virtual std::string
      label_(const label_t& l)
      {
        return (aut_.labelset()->is_one(l) ? "@epsilon"
                : format(*aut_.labelset(), l));
      }

      /// Output the transition \a t.  Do not insert eol.
      /// "Src Label Dst".
      virtual void output_transition_(transition_t t)
      {
        os_ << states_[aut_.src_of(t)]
            << ' ' << label_(aut_.label_of(t))
            << ' ' << states_[aut_.dst_of(t)];
      }

      /// The labels and weights of transitions from \a src to \a dst.
      ///
      /// The main advantage of using polynomials instead of directly
      /// iterating over aut_.outin(src, dst) is to get a result which
      /// is sorted (hence more deterministic).
      std::string format_entry_(state_t src, state_t dst)
      {
        auto entry = get_entry(aut_, src, dst);
        return ps_.format(entry, ", ");
      }

      /// Output transitions, sorted lexicographically on (Label, Dest).
      void output_state_(const state_t s)
      {
        std::vector<transition_t> ts;
        for (auto t : aut_.out(s))
          ts.push_back(t);
        std::sort
          (begin(ts), end(ts),
           [this](transition_t l, transition_t r)
           {
             return (std::forward_as_tuple(aut_.label_of(l), aut_.dst_of(l))
                     < std::forward_as_tuple(aut_.label_of(r), aut_.dst_of(r)));
           });
        for (auto t : ts)
          {
            os_ << '\n';
            output_transition_(t);
          }
      }

      /// Output transitions, sorted lexicographically.
      /// "Src Label Dst\n".
      void output_transitions_()
      {
        for (auto s: aut_.states())
          output_state_(s);
      }

      /// List names of states in \a ss, preceded by ' '.
      void list_states_(const states_t& ss)
      {
        for (auto s: ss)
          os_ << ' ' << states_[s];
      }

      /// The list of initial states, sorted.
      states_t initials_()
      {
        states_t res;
        for (auto t: aut_.initial_transitions())
          res.emplace_back(aut_.dst_of(t));
        std::sort(begin(res), end(res));
        return res;
      }

      /// The list of final states, sorted.
      states_t finals_()
      {
        states_t res;
        for (auto t: aut_.final_transitions())
          res.emplace_back(aut_.src_of(t));
        std::sort(begin(res), end(res));
        return res;
      }

      /// The automaton we have to output.
      const automaton_t& aut_;
      /// Output stream.
      std::ostream& os_;
      /// Short-hand to the weightset.
      const weightset_t& ws_ = *aut_.weightset();
      /// Short-hand to the polynomialset used to print the entries.
      const polynomialset<context_t> ps_{aut_.context()};
      /// Names (natural numbers) to use for the states.
      std::map<state_t, unsigned> states_;
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
    template <typename Aut>
    class fadoer: public outputter<Aut>
    {
      static_assert(Aut::context_t::is_lal || Aut::context_t::is_lan,
                    "requires labels_are_(letters|nullable)");
      // FIXME: Not right: F2 is also using bool, but it is not bool.
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using super_type = outputter<Aut>;

      using super_type::aut_;
      using super_type::finals_;
      using super_type::initials_;
      using super_type::list_states_;
      using super_type::os_;
      using super_type::output_transitions_;
      using super_type::ws_;

      using super_type::super_type;

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
        output_transitions_();
      }

    private:
      template <typename A>
      typename std::enable_if<A::context_t::labelset_t::is_free(),
                              bool>::type
      is_deterministic_(const A& a)
      {
        return vcsn::is_deterministic(a);
      }

      template <typename A>
      typename std::enable_if<!A::context_t::labelset_t::is_free(),
                              bool>::type
      is_deterministic_(const A&)
      {
        return false;
      }
    };

  }

  template <typename Aut>
  std::ostream&
  fado(const Aut& aut, std::ostream& out)
  {
    detail::fadoer<Aut> fado{aut, out};
    fado();
    return out;
  }


  namespace dyn
  {
    namespace detail
    {
      /*------------.
      | dyn::fado.  |
      `------------*/

      /// Bridge.
      template <typename Aut, typename Ostream>
      std::ostream& fado(const automaton& aut, std::ostream& out)
      {
        return fado(aut->as<Aut>(), out);
      }

      REGISTER_DECLARE(fado,
                        (const automaton& aut, std::ostream& out)
                        -> std::ostream&);

    }
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
    template <typename Aut>
    class grailer: public outputter<Aut>
    {
      static_assert(Aut::context_t::is_lal || Aut::context_t::is_lan,
                    "requires labels_are_(letters|nullable)");
      // FIXME: Not right: F2 is also using bool, but it is not bool.
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using super_type = outputter<Aut>;

      using typename super_type::automaton_t;
      using typename super_type::state_t;
      using typename super_type::transition_t;

      using super_type::aut_;
      using super_type::finals_;
      using super_type::initials_;
      using super_type::os_;
      using super_type::output_transitions_;
      using super_type::states_;
      using super_type::ws_;

      using super_type::super_type;

    public:
      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        // Don't end with a \n.
        const char* sep = "";
        for (auto s: initials_())
          {
            os_ << sep
                << "(START) |- "  << states_[s];
            sep = "\n";
          }
        output_transitions_();
        for (auto s: finals_())
          os_ << '\n'
              << states_[s] <<  " -| (FINAL)";
      }
    };
  }

  template <typename Aut>
  std::ostream&
  grail(const Aut& aut, std::ostream& out)
  {
    detail::grailer<Aut> grail{aut, out};
    grail();
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream>
      std::ostream& grail(const automaton& aut, std::ostream& out)
      {
        return grail(aut->as<Aut>(), out);
      }

      REGISTER_DECLARE(grail,
                        (const automaton& aut, std::ostream& out)
                        -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_GRAIL_HH
