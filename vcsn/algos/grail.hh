#ifndef VCSN_ALGOS_GRAIL_HH
# define VCSN_ALGOS_GRAIL_HH

# include <iostream>
# include <map>

# include <vcsn/algos/is-deterministic.hh>
# include <vcsn/dyn/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  namespace detail
  {

    /*------------.
    | outputter.  |
    `------------*/
    template <typename Aut>
    struct outputter
    {
      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;

      outputter(const automaton_t& aut, std::ostream& out)
        : aut_(aut)
        , os_(out)
      {
        unsigned s = 0;
        for (auto t: aut_.states())
          states_.emplace(t, s++);
      }

    protected:
      /// symbol lists.
      std::string
      label_of_(transition_t t)
      {
        const auto l = aut_.label_of(t);
        return
          aut_.labelset()->is_one(l) ? "@epsilon" : aut_.labelset()->format(l);
      }

      /// Output transitions, sorted lexicographically.
      /// "Src Label Dst\n".
      void output_transitions_()
      {
        std::vector<transition_t> order;
        for (auto t : aut_.transitions())
          order.push_back(t);
        std::sort (order.begin(), order.end(),
                   [this](transition_t l, transition_t r)
                   {
                     if (aut_.src_of(l) == aut_.src_of(r))
                       {
                         if (label_of_(l) == label_of_(r))
                           return aut_.dst_of(l) < aut_.dst_of(r);
                         else
                           return label_of_(l) < label_of_(r);
                       }
                     else
                       return aut_.src_of(l) < aut_.src_of(r);
                   });

        for (auto t : order)
          os_ << std::endl
              << states_[aut_.src_of(t)]
              << ' ' << label_of_(t)
              << ' ' << states_[aut_.dst_of(t)];
      }

      void output_finals_()
      {
        std::vector<unsigned> order;
        for (auto t: this->aut_.final_transitions())
          order.push_back(this->states_[this->aut_.src_of(t)]);
        std::sort(order.begin(), order.end());
        for (auto t: order)
          this->os_ << ' ' << t;
      }

      /// The automaton we have to output.
      const automaton_t& aut_;

      std::ostream& os_;
      std::map<state_t, unsigned> states_;
    };


    /*----------.
    | grailer.  |
    `----------*/

    // https://cs.uwaterloo.ca/research/tr/1993/01/93-01.pdf
    template <typename Aut>
    struct grailer: public outputter<Aut>
    {
      static_assert(Aut::context_t::is_lal | Aut::context_t::is_lan,
                    "requires labels_are_(letters|nullable)");
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using super_type = outputter<Aut>;

      using automaton_t = typename super_type::automaton_t;
      using state_t = typename super_type::state_t;
      using transition_t = typename super_type::transition_t;

      using super_type::super_type;

      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        // Don't end with a \n.
        const char* sep = "";
        for (auto t: this->aut_.initial_transitions())
          {
            this->os_ << sep << "(START) |- "  << this->states_[this->aut_.dst_of(t)];
            sep = "\n";
          }
        this->output_transitions_();
        for (auto t: this->aut_.final_transitions())
          this->os_  << std::endl << this->states_[this->aut_.src_of(t)] <<  " -| (FINAL)";
      }
    };


    /*---------.
    | fadoer.  |
    `---------*/

    // https://cs.uwaterloo.ca/research/tr/1993/01/93-01.pdf
    template <typename Aut>
    struct fadoer: public outputter<Aut>
    {
      static_assert(Aut::context_t::is_lal | Aut::context_t::is_lan,
                    "requires labels_are_(letters|nullable)");
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      using super_type = outputter<Aut>;

      using super_type::super_type;

      /// Actually output \a aut_ on \a os_.
      // http://www.dcc.fc.up.pt/~rvr/FAdoDoc/_modules/fa.html#saveToFile
      void operator()()
      {
        bool is_deter = is_deterministic_(this->aut_);
        this->os_ << (is_deter ? "@DFA" : "@NFA");
        this->output_finals_();
        if (!is_deter)
          output_initials_();
        this->output_transitions_();
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

      void output_initials_()
      {
        std::vector<unsigned> order;
        for (auto t: this->aut_.initial_transitions())
          order.push_back(this->states_[this->aut_.dst_of(t)]);
        std::sort(order.begin(), order.end());

        this->os_ << " *";
        for (auto t: order)
          this->os_ << ' ' << t;
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
        return fado(dynamic_cast<const Aut&>(*aut), out);
      }

      REGISTER_DECLARE(fado,
                       (const automaton& aut, std::ostream& out)
                       -> std::ostream&);


      /*-------------.
      | dyn::grail.  |
      `-------------*/
      /// Bridge.
      template <typename Aut>
      std::ostream& grail(const automaton& aut, std::ostream& out)
      {
        return grail(dynamic_cast<const Aut&>(*aut), out);
      }

      REGISTER_DECLARE(grail,
                       (const automaton& aut, std::ostream& out)
                       -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_GRAIL_HH
