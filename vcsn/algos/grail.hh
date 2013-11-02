#ifndef VCSN_ALGOS_GRAIL_HH
# define VCSN_ALGOS_GRAIL_HH

# include <iostream>
# include <map>

# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/escape.hh>
# include <vcsn/weights/polynomialset.hh>

namespace vcsn
{

  namespace detail
  {

    /*------------.
    | outputter.  |
    `------------*/
    template <typename Aut>
    class outputter
    {
    public:
      using automaton_t = Aut;
      using label_t = typename automaton_t::label_t;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename automaton_t::weight_t;

      outputter(const automaton_t& aut, std::ostream& out)
        : aut_(aut)
        , ws_(*aut_.weightset())
        , os_(out)
      {
        // Name the states.
        unsigned s = 0;
        for (auto t: aut_.states())
          states_.emplace(t, s++);
      }

    protected:
      /// Convert a label to its representation.
      virtual std::string
      label_(const label_t& l)
      {
        return (aut_.labelset()->is_one(l) ? "@epsilon"
                : aut_.labelset()->format(l));
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
        using context_t = typename automaton_t::context_t;
        static auto ps = polynomialset<context_t>{aut_.context()};

        auto entry = get_entry(aut_, src, dst);
        return ps.format(entry, ", ");
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
            os_ << std::endl;
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

      /// List states in \a states, preceded by ' '.
      void list_states_(const std::vector<state_t>& states)
      {
        std::vector<unsigned> ss;
        for (auto s: states)
          ss.push_back(states_[s]);
        std::sort(begin(ss), end(ss));
        for (auto s: ss)
          os_ << ' ' << s;
      }

      void output_initials_()
      {
        std::vector<state_t> ss;
        for (auto t: aut_.initial_transitions())
          ss.push_back(aut_.dst_of(t));
        list_states_(ss);
      }

      void output_finals_()
      {
        std::vector<unsigned> ss;
        for (auto t: aut_.final_transitions())
          ss.push_back(aut_.src_of(t));
        list_states_(ss);
      }

      /// The automaton we have to output.
      const automaton_t& aut_;
      /// Short-hand to the weightset.
      const weightset_t& ws_;
      /// Output stream.
      std::ostream& os_;
      /// Names (natural numbers) to use for the states.
      std::map<state_t, unsigned> states_;
    };

  }


  /*---------.
  | fadoer.  |
  `---------*/

  namespace detail
  {

    template <typename Aut>
    class fadoer: public outputter<Aut>
    {
      static_assert(Aut::context_t::is_lal | Aut::context_t::is_lan,
                    "requires labels_are_(letters|nullable)");
      // FIXME: Not right: F2 is also using bool, but it is not bool.
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using super_type = outputter<Aut>;

      using super_type::os_;
      using super_type::aut_;
      using super_type::ws_;
      using super_type::output_initials_;
      using super_type::output_finals_;
      using super_type::output_transitions_;

      using super_type::super_type;

    public:
      /// Actually output \a aut_ on \a os_.
      // http://www.dcc.fc.up.pt/~rvr/FAdoDoc/_modules/fa.html#saveToFile
      void operator()()
      {
        bool is_deter = is_deterministic_(aut_);
        os_ << (is_deter ? "@DFA" : "@NFA");
        output_finals_();
        if (!is_deter)
          {
            os_ << " *";
            output_initials_();
          }
        output_transitions_();
      }

    private:
      template <typename A>
      typename std::enable_if<A::context_t::is_lal,
                              bool>::type
      is_deterministic_(const A& a)
      {
        return vcsn::is_deterministic(a);
      }

      template <typename A>
      typename std::enable_if<A::context_t::is_lan,
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
      template <typename Aut>
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
    // https://cs.uwaterloo.ca/research/tr/1993/01/93-01.pdf
    template <typename Aut>
    class grailer: public outputter<Aut>
    {
      static_assert(Aut::context_t::is_lal | Aut::context_t::is_lan,
                    "requires labels_are_(letters|nullable)");
      // FIXME: Not right: F2 is also using bool, but it is not bool.
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using super_type = outputter<Aut>;

      using typename super_type::automaton_t;
      using typename super_type::state_t;
      using typename super_type::transition_t;

      using super_type::os_;
      using super_type::aut_;
      using super_type::ws_;
      using super_type::states_;
      using super_type::output_transitions_;

      using super_type::super_type;

    public:
      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        // Don't end with a \n.
        const char* sep = "";
        for (auto t: aut_.initial_transitions())
          {
            os_
              << sep
              << "(START) |- "  << states_[aut_.dst_of(t)];
            sep = "\n";
          }
        output_transitions_();
        for (auto t: aut_.final_transitions())
          os_
            << std::endl
            << states_[aut_.src_of(t)] <<  " -| (FINAL)";
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
      template <typename Aut>
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
