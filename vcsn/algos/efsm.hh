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
    /// http://www2.research.att.com/~fsmtools/fsm/man4/fsm.5.html
    template <typename Aut>
    class efsmer: public outputter<Aut>
    {
    protected:
      using automaton_t = Aut;
      using super_type = outputter<Aut>;

      using label_t = typename super_type::label_t;
      using state_t = typename super_type::state_t;
      using transition_t = typename super_type::transition_t;

      using super_type::os_;
      using super_type::aut_;
      using super_type::ls_;
      using super_type::ws_;

    public:
      using super_type::super_type;

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
          "cat >$medir/isymbols.txt <<\\EOFSM\n"
          ;
        output_isymbols_();
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
          "  --keep_isymbols --isymbols=$medir/isymbols.txt \\\n"
          "  --keep_osymbols --osymbols=$medir/isymbols.txt \\\n"
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
      using symbols_t = std::map<label_t, unsigned>;

      void output_transition_(const transition_t t)
      {
        aut_->print_state(aut_->src_of(t), os_);
        if (aut_->dst_of(t) != aut_->post())
          {
            os_ << '\t';
            aut_->print_state(aut_->dst_of(t), os_);
            os_ << '\t';
            if (ls_.is_special(aut_->label_of(t)))
              os_ << "\\e";
            else
              ls_.print(aut_->label_of(t), os_);
          }

        if (ws_.show_one() || !ws_.is_one(aut_->weight_of(t)))
          {
            os_ << '\t';
            ws_.print(aut_->weight_of(t), os_);
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

      /// Output the mapping from label name, to label number.  The
      /// FSM format uses integers for labels.
      void output_isymbols_()
      {
        symbols_t isymbols;

        // Find all the labels, to number them.
        {
          std::set<label_t> labels;
          for (auto t : aut_->transitions())
            labels.insert(aut_->label_of(t));
          // 0 is reserved for one and special.
          isymbols[ls_.special()] = 0;
          unsigned name_ = 1;
          for (auto l: labels)
            isymbols.emplace(l, ls_.is_one(l) ? 0 : name_++);
        }

        // Sorted per label name, which is fine, and deterministic.
        // Start with special/epsilon.  Show it as \e.
        os_ << "\\e\t0\n";
        for (const auto& p: isymbols)
          // Don't define 0 again.
          if (p.second)
            {
              ls_.print(p.first, os_);
              os_ << '\t' << p.second << '\n';
            }
      }
    };
  }


  /// \brief Format automaton to EFSM format, based on FSM format.
  ///
  /// \tparam AutPtr an automaton type.
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
