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

    // https://cs.uwaterloo.ca/research/tr/1993/01/93-01.pdf
    template <class Aut>
    struct grailer
    {
      static_assert(Aut::context_t::is_lal | Aut::context_t::is_lan,
                    "requires labels_are_(letters|nullable)");
      static_assert(std::is_same<typename Aut::weight_t, bool>::value,
                    "requires Boolean weights");

      enum class Output
      {
        FADO,
        GRAIL
      };

      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;

      grailer(const automaton_t& aut, std::ostream& out,
          const std::string& type)
        : aut_(aut)
        , os_(out)
      {
        if (type == "fado")
          type_ = Output::FADO;
        else
          type_ = Output::GRAIL;

        for (auto t: aut_.states())
          states_.emplace(t, state_++);
      }

      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        if (type_ == Output::FADO)
          fado_();
        else
          grail_();
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

      // http://www.dcc.fc.up.pt/~rvr/FAdoDoc/_modules/fa.html#saveToFile
      void fado_()
      {
        bool is_deter = is_deterministic_(aut_);
        os_ << (is_deter ? "@DFA" : "@NFA");
        output_finals_();
        if (!is_deter)
          output_initials_();
        output_transitions_();
      }

      void grail_()
      {
        auto it = aut_.initial_transitions().begin();
        auto end = aut_.initial_transitions().end();
        os_ << "(START) |- "  << states_[aut_.dst_of(*it)];
        std::advance (it, 1);
        while (it != end)
        {
          os_ << std::endl << "(START) |- "  << states_[aut_.dst_of(*it)];
          ++it;
        }
        output_transitions_();
        for (auto t: aut_.final_transitions())
          os_  << std::endl << states_[aut_.src_of(t)] <<  " -| (FINAL)";
      }

      void output_initials_()
      {
        std::vector<unsigned> order;
        for (auto t: aut_.initial_transitions())
          order.push_back(states_[aut_.dst_of(t)]);
        std::sort (order.begin(), order.end());

        os_ << " *";
        for (auto t: order)
          os_ << ' ' << t;
      }

      void output_finals_()
      {
        std::vector<unsigned> order;
        for (auto t: aut_.final_transitions())
          order.push_back(states_[aut_.src_of(t)]);
        std::sort (order.begin(), order.end());

	for (auto t: order)
	  os_ << ' ' << t;
      }

      /// symbol lists.
      std::string
      label_of_(transition_t t)
      {
        /// FIXME: not very elegant, \\e should be treated elsewhere.
        const auto l = aut_.label_of(t);
        std::string res =
          aut_.labelset()->is_one(l) ? "@epsilon" :
	  aut_.labelset()->format(l);

        return res;
      }

    void output_transitions_()
    {
      std::vector<transition_t> order;
      for ( auto t : aut_.transitions())
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

    /// The automaton we have to output.
    const automaton_t& aut_;

    std::ostream& os_;
    Output type_;
    std::map<state_t, unsigned> states_;
    unsigned state_ = 0;
  };
}


template <class Aut>
std::ostream&
grail(const Aut& aut, std::ostream& out, const std::string& type)
{
  detail::grailer<Aut> grail{aut, out, type};
  grail();
  return out;
}

namespace dyn
{
  namespace detail
  {
    template <typename Aut>
    std::ostream& grail(const automaton& aut, std::ostream& out,
                        const std::string& type)
    {
      return grail(dynamic_cast<const Aut&>(*aut), out, type);
    }

    REGISTER_DECLARE
      (grail,
        (const automaton& aut, std::ostream& out, const std::string& type)
        -> std::ostream&);
  }
}
}

#endif // !VCSN_ALGOS_GRAIL_HH
