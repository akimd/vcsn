#pragma once

#include <algorithm>
#include <iostream>

#include <boost/range/algorithm/sort.hpp>

#include <vcsn/algos/grail.hh> // printer
#include <vcsn/dyn/fwd.hh>
#include <vcsn/labelset/fwd.hh>
#include <vcsn/misc/escape.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{

  /*--------------------------.
  | efsm(automaton, stream).  |
  `--------------------------*/
  namespace detail
  {
    /// \brief Print automaton to EFSM format, based on FSM format.
    ///
    /// \tparam Aut an automaton type, not a pointer type.
    ///
    /// http://www2.research.att.com/~fsmtools/fsm/man4/fsm.5.html
    template <Automaton Aut>
    class efsmer: public printer<Aut>
    {
    protected:
      using automaton_t = Aut;
      using super_t = printer<Aut>;

      using label_t = typename super_t::label_t;
      using state_t = typename super_t::state_t;
      using transition_t = typename super_t::transition_t;

      using super_t::os_;
      using super_t::aut_;
      using super_t::ls_;
      using super_t::ws_;
      using super_t::is_transducer_;

    public:
      using super_t::super_t;

      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        // Get it before, as it could raise an invalid argument error during
        // the outputting.
        auto arc_type = arc_type_();

        os_ <<
          "#! /bin/sh\n"
          "\n"
          "me=$(basename \"$0\")\n"
          "medir=$(mktemp -d \"/tmp/$me.XXXXXX\") || exit 1\n"
          "\n";

        os_ <<
          "arc_type=" << arc_type << "\n"
          "\n";

          // Provide the symbols first, as when reading EFSM, knowing
          // how \e is represented will help reading the transitions.
        print_symbols_();

        os_ <<
          "cat >$medir/transitions.fsm <<\\EOFSM";
        print_transitions_();
        os_ <<
          "\n"
          "EOFSM\n"
          "\n"

          // Some OpenFST tools seem to require an output-symbol list,
          // even for acceptors.  While fstrmepsilon perfectly works
          // with just the isymbols, fstintersect (equivalent to our
          // conjunction) for instance, seems to require the osymbols;
          // this seems to be due to the fact that Open FST bases its
          // implementation of intersect on its (transducer)
          // composition.
          "fstcompile" << (is_transducer_ ? "" : " --acceptor") << " \\\n"
          "  --arc_type=$arc_type \\\n"
          "  --keep_isymbols --isymbols=" << isymbols_ << " \\\n"
          "  --keep_osymbols --osymbols=" << osymbols_ << " \\\n"
          "  $medir/transitions.fsm \"$@\"\n"
          "sta=$?\n"
          "\n"
          "rm -rf $medir\n"
          "exit $sta" // No final \n.
          ;
      }

    private:
      /// The OpenFST name that corresponds to our weightset.
      std::string arc_type_() const
      {
        static const auto map = getarg<std::string>
          {
            "weightset",
            {
              {"b",    "standard"},
              {"zmin", "standard"},
              {"rmin", "standard"},
              {"nmin", "standard"},
              {"log",  "log64"},
            }
          };
        return map[ws_.sname()];
      }

      template <typename LS>
      void print_label_(const LS& ls, const typename LS::value_t& l) const
      {
        if (ls.is_special(l))
          os_ << "\\e";
        else
          ls.print(l, os_, format::raw);
      }

      /// Acceptor.
      template <typename Label>
      void print_label_(const Label& l, std::false_type) const
      {
        print_label_(ls_, l);
      }

      /// Two-tape automaton.
      template <typename Label>
      void print_label_(const Label& l, std::true_type) const
      {
        print_label_(ls_.template set<0>(), std::get<0>(l));
        os_ << '\t';
        print_label_(ls_.template set<1>(), std::get<1>(l));
      }

      void print_transition_(const transition_t t) const override
      {
        // Don't output "pre", but an integer.
        // Previously, a very large unsigned integer was used. This introduced
        // an error in newer version of OpenFST, which read it as if it were a
        // negative number. Instead, the state number immediately after
        // the highest state number is used. There is a shift of 2 when printing
        // states number because of pre/post.
        if (aut_->src_of(t) == aut_->pre())
          os_ << states_size(aut_) - 2;
        else
          aut_->print_state(aut_->src_of(t), os_);
        if (aut_->dst_of(t) != aut_->post())
          {
            os_ << '\t';
            aut_->print_state(aut_->dst_of(t), os_);
            os_ << '\t';
            print_label_(aut_->label_of(t), is_transducer_);
          }

        if (ws_.show_one() || !ws_.is_one(aut_->weight_of(t)))
          {
            os_ << '\t';
            ws_.print(aut_->weight_of(t), os_);
          }
      }

      /// Output all the transitions, and final states.
      void print_transitions_()
      {
        // FSM format supports a single initial state with one as
        // weight.  This requires, when we have several initial
        // states or an non, to "exhibit" pre() and spontaneous transitions.
        // Avoid this when possible.
        auto inis =  initial_transitions(aut_);
        if (inis.size() != 1
            || !ws_.is_one(aut_->weight_of(inis.front())))
          for (auto t : inis)
            {
              os_ << '\n';
              print_transition_(t);
            }

        // We _must_ start by the initial state.
        {
          std::vector<state_t> states(std::begin(aut_->states()),
                                      std::end(aut_->states()));
          boost::sort(states,
                      [this](state_t l, state_t r)
                      {
                        return (std::forward_as_tuple(!aut_->is_initial(l), l)
                                < std::forward_as_tuple(!aut_->is_initial(r), r));
                      });
          for (auto s: states)
            this->print_state_(s);
        }
        for (auto t : final_transitions(aut_))
          {
            os_ << '\n';
            print_transition_(t);
          }
      }

      /// Fill \a labels with the gensets of \a ls.
      ///
      /// Case where `genset` is supported.
      ///
      /// \param ls
      ///    the labelset from which to pull the gensets
      /// \param labels
      ///    the set of labels to fill
      /// \param get_label
      ///     a projection to extract the label from a genset.  For instance
      ///     the genset might be pairs of labels, and get_label could select
      ///     first or second.
      template <typename LabelSet, typename Labels, typename GetLabel>
      auto add_alphabet_(const LabelSet& ls, Labels& labels, GetLabel get_label)
        -> std::enable_if_t<has_generators_mem_fn<LabelSet>{}>
      {
        for (auto l : ls.generators())
          labels.insert(get_label(ls.value(l)));
      }

      /// Fill \a ls with the letters.
      ///
      /// Case where `genset` is not supported: do nothing.
      template <typename LabelSet, typename Labels, typename GetLabel>
      auto add_alphabet_(const LabelSet&, Labels&, GetLabel)
        -> std::enable_if_t<!has_generators_mem_fn<LabelSet>{}>
      {}

      /// Output the mapping from label name, to label number.
      ///
      /// The FSM format uses integers for labels.  Reserve 0 for
      /// epsilon (and the special symbol, that flags initial and
      /// final transitions).
      ///
      /// Instead of directly printing the labels, use a projection
      /// function.  So when printing transducers, this function is
      /// used twice, once for each tape, with a projection function
      /// from two-tape labels to either one.
      ///
      /// \tparam LabelSet
      ///    the type of the labelset of the labels  to declare.
      /// \tparam GetLabel
      ///    the type of the lambda to apply to project the labels.
      ///
      /// \param name
      ///    name of the files to create (e.g., "isymbols.txt").
      /// \param ls
      ///    The LabelSet to use to print the labels.
      /// \param get_label
      ///    A projection from exact labels to the ones we output.
      ///    E.g., projections similar to "first" or "second" for
      ///    two-tape automata.
      template <typename LabelSet, typename GetLabel>
      void print_symbols_(const std::string& name,
                           const LabelSet& ls,
                           GetLabel get_label)
      {
        // The labels we declare.
        using labelset_t = LabelSet;
        using label_t = typename labelset_t::value_t;

        auto labels = std::set<label_t, vcsn::less<labelset_t>>{};
        // If there is an alphabet, include it, to preserve the
        // context in round trips.
        add_alphabet_(*aut_->labelset(), labels, get_label);
        // In any case, insert all our labels.
        for (auto t : transitions(aut_))
          labels.insert(get_label(aut_->label_of(t)));

        // Sorted per label name, which is fine, and deterministic.
        // Start with special/epsilon.  Show it as \e.
        os_ <<
          "cat >" << name << " <<\\EOFSM\n"
          "\\e\t0\n";
        size_t num = 0;
        for (const auto& l: labels)
          if (!ls.is_one(l))
            {
              ls.print(l, os_, format::raw);
              os_ << '\t' << ++num << '\n';
            }
        os_ <<
          "EOFSM\n"
          "\n";
      }

      /// Labels of an acceptor.
      template <typename>
      void
      print_symbols_impl_(std::false_type)
      {
        print_symbols_(isymbols_,
                        ls_,
                        [](label_t l) { return l; });
      }

      /// Labels of a two-tape automaton.
      template <typename>
      void
      print_symbols_impl_(std::true_type)
      {
        print_symbols_(isymbols_,
                        ls_.template set<0>(),
                        [](const label_t& l) { return std::get<0>(l); });
        print_symbols_(osymbols_,
                        ls_.template set<1>(),
                        [](const label_t& l) { return std::get<1>(l); });
      }

      void
      print_symbols_()
      {
        print_symbols_impl_<automaton_t>(is_transducer_);
      }

      /// File name for input tape symbols.
      const char* isymbols_ =
        is_transducer_ ? "$medir/isymbols.txt" : "$medir/symbols.txt";
      /// File name for output tape symbols.
      const char* osymbols_ =
        is_transducer_ ? "$medir/osymbols.txt" : "$medir/symbols.txt";
    };
  }


  /// \brief Print automaton to EFSM format, based on FSM format.
  ///
  /// \tparam Aut an automaton type.
  template <Automaton Aut>
  std::ostream&
  efsm(const Aut& aut, std::ostream& out = std::cout)
  {
    auto efsm = detail::efsmer<Aut>{aut, out};
    efsm();
    return out;
  }
}
