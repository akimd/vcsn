#ifndef VCSN_ALGOS_EFSM_HH
# define VCSN_ALGOS_EFSM_HH

# include <algorithm>
# include <iostream>
# include <map>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/algos/grail.hh> // outputter

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
    struct efsmer: public outputter<Aut>
    {
      using automaton_t = Aut;
      using super_type = outputter<Aut>;
      using super_type::aut_;
      using super_type::os_;
      using super_type::states_;

      using label_t = typename automaton_t::label_t;
      using transition_t = typename automaton_t::transition_t;
      using state_t = typename automaton_t::state_t;

      efsmer(const automaton_t& aut, std::ostream& out)
        : super_type(aut, out)
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

        // Provide the symbols first, as when reading EFSM, knowing
        // how \e is represented will help reading the transitions.
        os_ << "cat >symbols.txt <<\\EOFSM" << std::endl;
        output_input_labels_();
        os_ << "EOFSM" << std::endl
            << std::endl;

        os_ << "cat >transitions.fsm <<\\EOFSM";
        output_transitions_();
        os_ << std::endl
            << "EOFSM" << std::endl
            << std::endl;

        // Some OpenFST tools seem to really require an output-symbol
        // list, even for acceptors.  While fstrmepsilon perfectly
        // works with just the isymbols, fstintersect (equivalent to
        // our vcsn-product: the Hadamard product) for instance, seems
        // to require the osymbols; this seems to be due to the fact
        // that Open FST bases its implementation of intersect on its
        // (transducer) composition.
        os_ << "fstcompile --acceptor \\" << std::endl
            << "  --keep_isymbols --isymbols=symbols.txt \\" << std::endl
            << "  --keep_osymbols --osymbols=symbols.txt \\" << std::endl
            << "  transitions.fsm \"$@\"";
      }

    private:
      /// The FSM format uses integers for labels.  Reserve 0 for
      /// epsilon (and the special symbol, that flags initial and
      /// final transitions).
      using label_names_t = std::map<std::string, unsigned>;

      /// Return the label of transition \a t, record it for the input
      /// symbol lists.
      virtual std::string
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

      void output_transition_(const transition_t t)
      {
        const auto& ws = *aut_.weightset();
        bool show_one = ws.show_one();
        os_ << states_[aut_.src_of(t)];
        if (aut_.dst_of(t) != aut_.post())
          os_ << '\t' << states_[aut_.dst_of(t)]
              << '\t' << label_of_(t);

        if (show_one || !ws.is_one(aut_.weight_of(t)))
          {
            os_ << '\t';
            ws.print(os_, aut_.weight_of(t));
          }
      }

      /// Output all the transitions, and final states.
      void output_transitions_()
      {
        // FSM format supports a single initial state, which requires,
        // when we have several initial states, to "exhibit" pre() and
        // spontaneous transitions.  Avoid this when possible.
        if (aut_.initial_transitions().size() != 1)
          for (auto t : aut_.initial_transitions())
            {
              os_ << std::endl;
              output_transition_(t);
            }

        // We _must_ start by the initial state.
        {
          std::vector<state_t> states(std::begin(aut_.states()),
                                      std::end(aut_.states()));
          std::sort(begin(states), end(states),
                    [this](state_t l, state_t r)
                    {
                      return (std::forward_as_tuple(!aut_.is_initial(l), l)
                              < std::forward_as_tuple(!aut_.is_initial(r), r));
                    });
          for (auto s: states)
            this->output_state_(s);
        }
        for (auto t : aut_.final_transitions())
          {
            os_ << std::endl;
            output_transition_(t);
          }
      }

      /// Output the mapping from label name, to label number.
      void output_input_labels_()
      {
        // Required to print all labels.
        // FIXME: sorting the whole set of transitions.
        // Instead, loop first one states, then on transitions.
        {
          std::vector<transition_t> v(std::begin(aut_.all_transitions()),
                                      std::end(aut_.all_transitions()));
          std::sort(begin(v), end(v),
            [this](transition_t l, transition_t r)
            {
              return aut_.label_of(l) < aut_.label_of(r);
            });
          for (auto t : v)
            label_of_(t);
        }
        // Sorted per label name, which is fine, and deterministic.
        // Start with special/epsilon.  Show it as \e.
        os_ << "\\e\t0" << std::endl;
        for (const auto& p: names_)
          // Don't define 0 again.
          if (p.second)
            os_ << p.first << '\t' << p.second << std::endl;
      }

      /// The FSM format uses integers for labels.
      label_names_t names_;
      /// A counter used to name the labels.
      /// 0 is already used for epsilon and special.
      unsigned name_ = 1;
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
        return efsm(aut->as<Aut>(), out);
      }

      REGISTER_DECLARE(efsm,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_EFSM_HH
