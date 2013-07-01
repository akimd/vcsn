#ifndef VCSN_ALGOS_EFSM_HH
# define VCSN_ALGOS_EFSM_HH

# include <iostream>
# include <map>

# include <vcsn/dyn/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  /*--------------------------.
  | efsm(automaton, stream).  |
  `--------------------------*/
  namespace detail
  {

    /// http://www2.research.att.com/~efsmtools/efsm/man4/efsm.5.html
    template <class Aut>
    struct efsmer
    {
      using automaton_t = Aut;
      using label_t = typename automaton_t::label_t;
      using transition_t = typename automaton_t::transition_t;

      efsmer(const automaton_t& aut, std::ostream& out)
        : aut_(aut)
        , os_(out)
      {
        // Special label.
        names_[""] = 0;
        // Empty label.
        names_["\\e"] = 0;
      }

      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        os_ << "#! /bin/sh" << std::endl
            << std::endl;

        os_ << "cat >transitions.fsm <<\\EOFSM" << std::endl;
        output_transitions_();
        os_ << "EOFSM" << std::endl
            << std::endl;

        os_ << "cat >isymbols.txt <<\\EOFSM" << std::endl;
        output_input_labels_();
        os_ << "EOFSM" << std::endl
            << std::endl;

        os_ << "fstcompile --acceptor"
            << " --keep_isymbols --isymbols=isymbols.txt"
            << " transitions.fsm";
      }

    private:
      /// The FSM format uses integers for labels.  Reserve 0 for
      /// epsilon (and the special symbol, that flags initial and
      /// final transitions).
      using label_names_t = std::map<std::string, unsigned>;

      /// Return the label of transition \a t, record it for the input
      /// symbol lists.
      std::string
      label_of_(transition_t t)
      {
        /// FIXME: not very elegant, \\e should be treated elsewhere.
        const auto l = aut_.label_of(t);
        std::string res =
          aut_.labelset()->is_special(l) ? "\\e" : aut_.labelset()->format(l);

        auto insert = names_.emplace(res, name_);
        // If the label is fresh, prepare the next name.
        if (insert.second)
          ++name_;
        return res;
      }

      /// Output all the transitions, and final states.
      void output_transitions_()
      {
        const auto& ws = *aut_.weightset();
        bool show_one = ws.show_one();
        for (auto t : aut_.all_transitions())
          {
            os_ << aut_.src_of(t);
            if (aut_.dst_of(t) != aut_.post())
              os_ << '\t' << aut_.dst_of(t)
                  << '\t' << label_of_(t);

            if (show_one || !ws.is_one(aut_.weight_of(t)))
              {
                os_ << '\t';
                ws.print(os_, aut_.weight_of(t));
              }
            os_ << std::endl;
          }
      }

      /// Output the mapping from label name, to label number.
      void output_input_labels_()
      {
        // Sorted per label name, which is fine, and deterministic.
        // Start with special/epsilon.  Show it as \e.
        os_ << "\\e\t0" << std::endl;
        for (const auto& p: names_)
          // Don't define 0 again.
          if (p.second)
            os_ << p.first << '\t' << p.second << std::endl;
      }

      /// The automaton we have to output.
      const automaton_t& aut_;
      /// The FSM format uses integers for labels.
      label_names_t names_;
      /// A counter used to name the labels.
      /// 0 is already used for epsilon and special.
      unsigned name_ = 1;

      std::ostream& os_;
    };
  }



  // http://www2.research.att.com/~efsmtools/efsm/man4/efsm.5.html
  template <class Aut>
  std::ostream&
  efsm(const Aut& aut, std::ostream& out)
  {
    detail::efsmer<Aut> efsm{aut, out};
    efsm();
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      std::ostream& efsm(const automaton& aut, std::ostream& out)
      {
        return efsm(dynamic_cast<const Aut&>(*aut), out);
      }

      REGISTER_DECLARE(efsm,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_EFSM_HH
