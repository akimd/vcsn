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

    /// \brief Format automaton to EFSM format, based on FSM format.
    ///
    /// \tparam Aut an automaton type, not a pointer type.
    ///
    /// http://www2.research.att.com/~efsmtools/efsm/man4/efsm.5.html
    template <typename Aut>
    class efsmer: public outputter<Aut>
    {
    protected:
      using automaton_t = Aut;
      using super_type = outputter<Aut>;

      using typename super_type::label_t;
      using typename super_type::transition_t;

      using super_type::os_;
      using super_type::aut_;
      using super_type::ws_;
      using super_type::states_;

    public:
      using typename super_type::state_t;

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
        os_ <<
          "#! /bin/sh\n"
          "\n"
          "me=$(basename \"$0\")\n"
          "medir=$(mktemp -d \"/tmp/$me.XXXXXX\") || exit 1\n"
          "\n"

          // Provide the symbols first, as when reading EFSM, knowing
          // how \e is represented will help reading the transitions.
          "cat >$medir/symbols.txt <<\\EOFSM\n"
          ;
        output_input_labels_();
        os_ <<
          "EOFSM\n"
          "\n"

          "cat >$medir/transitions.fsm <<\\EOFSM";
        output_transitions_();
        os_ <<
          "\n"
          "EOFSM\n"
          "\n"

          // Some OpenFST tools seem to really require an
          // output-symbol list, even for acceptors.  While
          // fstrmepsilon perfectly works with just the isymbols,
          // fstintersect (equivalent to our vcsn-product: the
          // Hadamard product) for instance, seems to require the
          // osymbols; this seems to be due to the fact that Open FST
          // bases its implementation of intersect on its (transducer)
          // composition.
          "fstcompile --acceptor \\\n"
          "  --keep_isymbols --isymbols=$medir/symbols.txt \\\n"
          "  --keep_osymbols --osymbols=$medir/symbols.txt \\\n"
          "  $medir/transitions.fsm \"$@\"\n"
          "sta=$?\n"
          "\n"
          "rm -rf $medir\n"
          "exit $sta" // No final \n.
          ;
      }

    private:
      /// The FSM format uses integers for labels.  Reserve 0 for
      /// epsilon (and the special symbol, that flags initial and
      /// final transitions).
      using label_names_t = std::map<std::string, unsigned>;

      /// Return the label \a l, record it for the input symbol lists.
      virtual std::string
      label_(const label_t& l)
      {
        /// FIXME: not very elegant, \\e should be treated elsewhere.
        std::string res = (aut_->labelset()->is_special(l) ? "\\e"
                           : format(*aut_->labelset(), l));
        auto insert = names_.emplace(res, name_);
        // If the label is fresh, prepare the next name.
        if (insert.second)
          ++name_;
        return res;
      }

      void output_transition_(const transition_t t)
      {
        os_ << states_[aut_->src_of(t)];
        if (aut_->dst_of(t) != aut_->post())
          os_ << '\t' << states_[aut_->dst_of(t)]
              << '\t' << label_(aut_->label_of(t));

        if (ws_.show_one() || !ws_.is_one(aut_->weight_of(t)))
          {
            os_ << '\t';
            ws_.print(os_, aut_->weight_of(t));
          }
      }

      /// Output all the transitions, and final states.
      void output_transitions_()
      {
        // FSM format supports a single initial state, which requires,
        // when we have several initial states, to "exhibit" pre() and
        // spontaneous transitions.  Avoid this when possible.
        if (aut_->initial_transitions().size() != 1)
          for (auto t : aut_->initial_transitions())
            {
              os_ << '\n';
              output_transition_(t);
            }

        // We _must_ start by the initial state.
        {
          std::vector<state_t> states(std::begin(aut_->states()),
                                      std::end(aut_->states()));
          std::sort(begin(states), end(states),
                    [this](state_t l, state_t r)
                    {
                      return (std::forward_as_tuple(!aut_->is_initial(l), l)
                              < std::forward_as_tuple(!aut_->is_initial(r), r));
                    });
          for (auto s: states)
            this->output_state_(s);
        }
        for (auto t : aut_->final_transitions())
          {
            os_ << '\n';
            output_transition_(t);
          }
      }

      /// Output the mapping from label name, to label number.
      void output_input_labels_()
      {
        // Find all the labels, to number them.
        {
          std::set<label_t> labels;
          for (auto t : aut_->transitions())
            labels.insert(aut_->label_of(t));
          for (auto l: labels)
            label_(l);
        }
        // Sorted per label name, which is fine, and deterministic.
        // Start with special/epsilon.  Show it as \e.
        os_ << "\\e\t0\n";
        for (const auto& p: names_)
          // Don't define 0 again.
          if (p.second)
            os_ << p.first << '\t' << p.second << '\n';
      }

      /// The FSM format uses integers for labels.
      label_names_t names_;
      /// A counter used to name the labels.
      /// 0 is already used for epsilon and special.
      unsigned name_ = 1;
    };
  }


  /// \brief Format automaton to EFSM format, based on FSM format.
  ///
  /// \tparam Aut an automaton type, not a pointer type.
  template <typename AutPtr>
  std::ostream&
  efsm(const AutPtr& aut, std::ostream& out)
  {
    detail::efsmer<AutPtr> efsm{aut, out};
    efsm();
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream>
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
