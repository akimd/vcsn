#ifndef VCSN_ALGOS_COPY_HH
# define VCSN_ALGOS_COPY_HH

# include <unordered_map>

# include <vcsn/algos/fwd.hh> // blind_automaton.
# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/fwd.hh>
# include <vcsn/core/rat/copy.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/dyn/ratexpset.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/set.hh>

namespace vcsn
{

  /*--------------------------.
  | sub automaton decorator.  |
  `--------------------------*/
  namespace detail
  {
    /// \brief The sub automaton.
    /// Copy \a automaton type Aut1 to sub_automaton
    /// Or copy \a automaton type Aut1 to other automaton type Aut2
    template <typename Aut1,
              typename Aut2 = mutable_automaton<context_t_of<Aut1>>>
    class sub_automaton_impl
      : public automaton_decorator<Aut2>
    {
    public:
      using automaton_t = Aut1;
      using automaton_nocv_t = Aut2;
      using super_t = automaton_decorator<automaton_nocv_t>;
      using state_t = state_t_of<automaton_t>;

      sub_automaton_impl(const automaton_t& input, automaton_nocv_t res)
        : super_t(res)
        , input_(input)
      {}

      sub_automaton_impl(const automaton_t& input)
        : super_t(input->context())
        , input_(input)
      {}

      template <typename Pred>
      void operator()(Pred keep_state)
      {
        // Copy the states.  We cannot iterate on the transitions
        // only, as we would lose the states without transitions.
        map_[input_->pre()] = this->pre();
        map_[input_->post()] = this->post();
        for (auto s: input_->states())
          if (keep_state(s))
            map_[s] = this->new_state();

        for (auto t : input_->all_transitions())
          {
            auto src = map_.find(input_->src_of(t));
            auto dst = map_.find(input_->dst_of(t));
            if (src != map_.end() && dst != map_.end())
              this->new_transition_copy(src->second, dst->second,
                                        input_, t);
          }
      }

      static std::string sname()
      {
        return "sub_automaton<" + automaton_t::element_type::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "sub_automaton<" + input_->vname(full) + ">";
      }

      bool state_has_name(state_t s) const
      {
        return (s != super_t::pre()
                && s != super_t::post()
                && has(origins(), s));
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       const std::string& fmt = "text") const
      {
        auto i = origins().find(s);
        if (i == origins().end())
            this->print_state(s, o);
        else
          input_->print_state_name(i->second, o, fmt);
        return o;
      }

      /// A map from result state to original state.
      using origins_t = std::map<state_t, state_t>;
      mutable origins_t origins_;
      const origins_t& origins() const
      {
        if (origins_.empty())
          for (const auto& p : map_)
            origins_.emplace(p.second, p.first);
        return origins_;
      }

    private:
      std::map<state_t, state_t> map_;

      /// Input automaton.
      automaton_t input_;
    };
  }

  template <typename Aut1,
            typename Aut2 = mutable_automaton<context_t_of<Aut1>>>
  using sub_automaton
  = std::shared_ptr<detail::sub_automaton_impl<Aut1, Aut2>>;

  template <typename Aut1,
            typename Aut2,
            typename Pred>
  inline
  void
  copy_into(const Aut1& in, Aut2& out, Pred keep_state)
  {
    auto res = make_shared_ptr<sub_automaton<Aut1, Aut2>>(in, out);
    (*res)(keep_state);
  }

  template <typename Aut1,
            typename Aut2>
  inline
  void
  copy_into(const Aut1& in, Aut2& out)
  {
    copy_into(in, out, [](state_t_of<Aut1>) { return true; });
  }

  template <typename Aut,
            typename Pred>
  inline
  sub_automaton<Aut>
  copy(const Aut& a, Pred keep_state)
  {
    auto res = make_shared_ptr<sub_automaton<Aut>>(a);
    (*res)(keep_state);
    return res;
  }


  template <typename Aut>
  inline
  sub_automaton<Aut>
  copy(const Aut& a)
  {
    auto res = make_shared_ptr<sub_automaton<Aut>>(a);
    (*res)([](state_t_of<Aut>) { return true; });
    return res;
  }

  template <typename Aut>
  inline
  sub_automaton<Aut>
  copy(const Aut& a, const std::set<state_t_of<Aut>>& keep)
  {
    auto res = make_shared_ptr<sub_automaton<Aut>>(a);
    (*res)([&keep](state_t_of<Aut> s) { return has(keep, s); });
    return res;
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


  namespace detail
  {
    template <typename Aut>
    struct real_context_impl;

    template <typename Aut>
    auto
    real_context(const Aut& aut)
      -> decltype(real_context_impl<Aut>::context(aut));

    template <typename Aut>
    struct real_context_impl
    {
      static auto context(const Aut& aut)
        -> decltype(aut->context())
      {
        return aut->context();
      }
    };

    template <std::size_t Tape, typename Aut>
    struct real_context_impl<blind_automaton<Tape, Aut>>
    {
      static auto context(const blind_automaton<Tape, Aut>& aut)
        -> decltype(aut->full_context())
      {
        return aut->full_context();
      }
    };

    template <typename Aut>
    struct real_context_impl<permutation_automaton<Aut>>
    {
      static auto context(const permutation_automaton<Aut>& aut)
        -> decltype(real_context(aut->strip()))
      {
        return real_context(aut->strip());
      }
    };

    template <typename Aut>
    auto
    real_context(const Aut& aut)
      -> decltype(real_context_impl<Aut>::context(aut))
    {
      return real_context_impl<Aut>::context(aut);
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
