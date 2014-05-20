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
      using in_state_t = state_t_of<AutIn>;
      using out_state_t = state_t_of<AutOut>;

      copier(const AutIn& in, AutOut& out)
        : in_(in)
        , out_(out)
      {}

      template <typename Pred>
      void operator()(Pred keep_state)
      {
        const auto& out_ls = *out_.labelset();
        const auto& in_ls = *in_.labelset();
        const auto& out_ws = *out_.weightset();
        const auto& in_ws = *in_.weightset();

        // Copy the states.  We cannot iterate on the transitions
        // only, as we would lose the states without transitions.
        out_state[in_.pre()] = out_.pre();
        out_state[in_.post()] = out_.post();
        for (auto s: in_.states())
          if (keep_state(s))
            out_state[s] = out_.new_state();

        for (auto t : in_.all_transitions())
          {
            auto src = out_state.find(in_.src_of(t));
            auto dst = out_state.find(in_.dst_of(t));
            if (src != out_state.end() && dst != out_state.end())
              out_.new_transition(src->second, dst->second,
                                  out_ls.conv(in_ls, in_.label_of(t)),
                                  out_ws.conv(in_ws, in_.weight_of(t)));
          }
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

      /// Input automaton.
      const AutIn& in_;
      /// Output automaton.
      AutOut& out_;
      /// input state -> output state.
      std::unordered_map<in_state_t, out_state_t> out_state;
    };
  }

  /// Copy an automaton.
  /// \precondition AutIn <: AutOut.
  template <typename AutIn, typename AutOut, typename Pred>
  inline
  void
  copy_into(const AutIn& in, AutOut& out, Pred keep_state)
  {
    detail::copier<AutIn, AutOut> copy(in, out);
    return copy(keep_state);
  }

  template <typename AutIn, typename AutOut>
  inline
  void
  copy_into(const AutIn& in, AutOut& out)
  {
    return copy_into(in, out, [](state_t_of<AutIn>) { return true; });
  }

  /// A copy of \a input keeping only its states that are accepted by
  /// \a keep_state.
  template <typename AutIn, typename AutOut = AutIn, typename Pred>
  inline
  AutOut
  copy(const AutIn& input, Pred keep_state)
  {
    AutOut res{input.context()};
    ::vcsn::copy_into(input, res, keep_state);
    return res;
  }

  /// A copy of \a input.
  template <typename AutIn, typename AutOut = AutIn>
  inline
  AutOut
  copy(const AutIn& input)
  {
    return ::vcsn::copy(input,
                        [](state_t_of<AutIn>) { return true; });
  }

  /// A copy of \a input keeping only its states that are members of
  /// \a keep.
  template <typename Aut>
  inline
  Aut
  copy(const Aut& input, const std::set<state_t_of<Aut>>& keep)
  {
    return ::vcsn::copy(input,
                        [&keep](state_t_of<Aut> s)
                        {
                          return has(keep, s);
                        });
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      inline
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
      inline
      ratexp
      copy_ratexp(const ratexp& exp, const ratexpset& out_rs)
      {
        const auto& r = exp->as<InRatExpSet>();
        const auto& ors = out_rs->as<OutRatExpSet>().ratexpset();

        return make_ratexp(ors,
                           ::vcsn::rat::copy(r.ratexpset(), ors,
                                             r.ratexp()));
      }

      REGISTER_DECLARE(copy_ratexp,
                       (const ratexp& exp, const ratexpset& out_rs) -> ratexp);
    }
  }



} // namespace vcsn

#endif // !VCSN_ALGOS_COPY_HH
