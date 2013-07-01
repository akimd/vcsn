#ifndef VCSN_ALGOS_FSM_HH
# define VCSN_ALGOS_FSM_HH

# include <iostream>
# include <unordered_map>

# include <vcsn/core/kind.hh>
# include <vcsn/dyn/fwd.hh>

# include <vcsn/misc/escape.hh>

namespace vcsn
{

  /*-------------------------.
  | fsm(automaton, stream).  |
  `-------------------------*/
  namespace detail
  {

    // http://www2.research.att.com/~fsmtools/fsm/man4/fsm.5.html
    template <class Aut>
    struct fsmer
    {
      using automaton_t = Aut;
      using label_t = typename automaton_t::label_t;

      fsmer(const automaton_t& aut, std::ostream& out)
        : aut_(aut)
        , os_(out)
      {
        // Map epsilon and the special symbol (that labels initial and
        // final transitions) to '0'.  Requested by the FSM format.
        names_[aut_.labelset()->special()] = 0;
        map_one_to_zero_(*aut_.labelset());
      }

      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        output_transitions_();
      }

    private:
      // The FSM format uses integers for labels.  Reserve 0 for epsilon
      // (and the special symbol, that flags initial and final
      // transitions).
      using label_names_t = std::unordered_map<label_t, unsigned>;

      /// If the "one" label exists, map it to 0 in \a map_.
      template <typename LabelSet>
      typename std::enable_if<is_lal<LabelSet>::value>::type
      map_one_to_zero_(const LabelSet&)
      {}

      template <typename LabelSet>
      typename std::enable_if<!is_lal<LabelSet>::value>::type
      map_one_to_zero_(const LabelSet& ls)
      {
        names_[ls.one()] = 0;
      }

      void output_transitions_()
      {
        // A counter used to name the labels.
        // 0 is already used for epsilon and special.
        unsigned name = 1;

        for (auto t : aut_.all_transitions())
          {
            const auto& lbl = aut_.label_of(t);
            auto insert = names_.emplace(lbl, name);
            if (insert.second)
              ++name;
            os_ << aut_.src_of(t) << '\t'
                << aut_.dst_of(t) << '\t'
                << insert.first->second // << '\t'
              //            << 0 // Output label: epsilon.
              // FIXME: aut.weight_of(t)
                << std::endl;
          }
        // post is the only final state.
        os_ << aut_.post();
      }

      /// The automaton we have to output.
      const automaton_t& aut_;
      /// The FSM format uses integers for labels.
      label_names_t names_;
      std::ostream& os_;
    };
  }



  // http://www2.research.att.com/~fsmtools/fsm/man4/fsm.5.html
  template <class Aut>
  std::ostream&
  fsm(const Aut& aut, std::ostream& out)
  {
    detail::fsmer<Aut> fsm{aut, out};
    fsm();
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      std::ostream& fsm(const automaton& aut, std::ostream& out)
      {
        return fsm(dynamic_cast<const Aut&>(*aut), out);
      }

      REGISTER_DECLARE(fsm,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_FSM_HH
