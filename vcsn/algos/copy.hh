#ifndef VCSN_ALGOS_COPY_HH
# define VCSN_ALGOS_COPY_HH

# include <unordered_map>

# include <vcsn/core/fwd.hh>
# include <vcsn/core/rat/copy.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/ratexpset.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/set.hh>

namespace vcsn
{

  /*------------------.
  | copy(automaton).  |
  `------------------*/

  namespace detail
  {
    /// Copy an automaton.
    /// \precondition AutIn <: AutOut.
    template <typename AutIn, typename AutOut>
    struct copier
    {
      using in_state_t = typename AutIn::state_t;
      using out_state_t = typename AutOut::state_t;

      // FIXME: GCC does not seem to like that we define keep_state
      // with a default assignment.  Check and posssibly report.
      copier()
        : keep_state([](in_state_t) { return true; })
      {}

      void operator()(const AutIn& in, AutOut& out)
      {
        const auto& out_ls = *out.labelset();
        const auto& in_ls = *in.labelset();
        const auto& out_ws = *out.weightset();
        const auto& in_ws = *in.weightset();

        for (auto s : in.states())
          if (keep_state(s))
            {
              out_state_t ns =  out.new_state();
              out_state[s] = ns;
              out.set_initial(ns, out_ws.conv(in_ws, in.get_initial_weight(s)));
              out.set_final(ns, out_ws.conv(in_ws, in.get_final_weight(s)));
            }
        for (auto t : in.transitions())
          if (keep_state(in.src_of(t))
              && keep_state(in.dst_of(t)))
            out.set_transition(out_state[in.src_of(t)],
                               out_state[in.dst_of(t)],
                               out_ls.conv(in_ls, in.label_of(t)),
                               out_ws.conv(in_ws, in.weight_of(t)));
      }

      /// A map from result state to original state.
      using origins_t = std::map<out_state_t, in_state_t>;
      origins_t
      origins() const
      {
        origins_t res;
        for (const auto& p: out_state)
          res[p.second] = p.first;
        return res;
      }

      /// Print the origins.
      static
      std::ostream&
      print(std::ostream& o, const origins_t& orig)
      {
        o << "/* Origins.\n"
          << "    node [shape = box, style = rounded]\n";
        for (auto p : orig)
          if (2 <= p.first)
            o << "    " << p.first - 2
              << " [label = \"" << p.second - 2 << "\"]\n";
        o << "*/\n";
        return o;
      }

      /// Whether to keep a state (and its transitions) in the result.
      std::function<bool(in_state_t)> keep_state;
      /// input state -> output state.
      std::unordered_map<in_state_t, out_state_t> out_state;
    };
  }

  /// Copy an automaton.
  /// \precondition AutIn <: AutOut.
  template <typename AutIn, typename AutOut>
  void
  copy(const AutIn& in, AutOut& out,
       std::function<bool(typename AutIn::state_t)> keep_state)
  {
    detail::copier<AutIn, AutOut> copy;
    copy.keep_state = keep_state;
    return copy(in, out);
  }

  template <typename State>
  bool
  keep_all_states(State) ATTRIBUTE_CONST;

  template <typename State>
  bool
  keep_all_states(State)
  {
    return true;
  }

  /// A copy of \a input keeping only its states that are accepted by
  /// \a keep_state.
  template <typename Aut>
  Aut
  copy(const Aut& input,
       std::function<bool(typename Aut::state_t)> keep_state)
  {
    using automaton_t = Aut;
    automaton_t output{input.context()};
    /// Beware of clashes with std::copy.
    ::vcsn::copy(input, output, keep_state);
    return output;
  }

  /// Convenience wrapper for lambdas for instance.
  template <typename Aut, typename StatePred>
  Aut
  copy(const Aut& input, StatePred keep_state)
  {
    return::vcsn::copy(input,
                       std::function<bool(typename Aut::state_t)>{keep_state});
  }

  /// A copy of \a input keeping only its states that are members of
  /// \a keep.
  template <typename Aut>
  Aut
  copy(const Aut& input, const std::set<typename Aut::state_t>& keep)
  {
    using state_t = typename Aut::state_t;
    return ::vcsn::copy(input, [&keep](state_t s)
                        {
                          return has(keep, s);
                        });
  }

  /// Clone \a input.
  // FIXME: Is there a means to use default arguments?
  template <typename Aut>
  Aut
  copy(const Aut& input)
  {
    return ::vcsn::copy(input, {keep_all_states<typename Aut::state_t>});
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      copy(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::copy(a));
      }

      REGISTER_DECLARE(copy,
                       (const automaton&) -> automaton);
    }
  }

  /*---------------.
  | copy(ratexp).  |
  `---------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename InRatExpSet, typename OutRatExpSet = InRatExpSet>
      ratexp
      copy_ratexp(const ratexp& exp, const ratexpset& out_rs)
      {
        const auto& r = exp->as<InRatExpSet>();
        const auto& ors = out_rs->as<OutRatExpSet>().get_ratexpset();

        return make_ratexp(ors,
                           ::vcsn::rat::copy(r.get_ratexpset(), ors,
                                             r.ratexp()));
      }

      REGISTER_DECLARE(copy_ratexp,
                       (const ratexp& exp, const ratexpset& out_rs) -> ratexp);
    }
  }



} // namespace vcsn

#endif // !VCSN_ALGOS_COPY_HH
