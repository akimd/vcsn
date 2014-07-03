#ifndef VCSN_ALGOS_BLIND_HH
# define VCSN_ALGOS_BLIND_HH

# include <vcsn/algos/fwd.hh>
# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/labelset/tupleset.hh>
# include <vcsn/misc/tuple.hh> // make_index_range

namespace vcsn
{
  namespace detail
  {
    template <typename A, typename I>
    struct hidden_label_type;

    template <typename Aut, std::size_t... I>
    struct hidden_label_type<Aut, index_sequence<I...>>
    {
      template <std::size_t J>
      using elem = typename std::tuple_element<J,
            typename labelset_t_of<Aut>::valuesets_t>::type;

      using type = tupleset<elem<I>...>;
    };


    template <size_t Tape,
              typename LabelSet, typename WeightSet>
    auto blind_context(const context<LabelSet, WeightSet>& ctx)
      -> context<typename LabelSet::template valueset_t<Tape>, WeightSet>
    {
      return {ctx.labelset()->template set<Tape>(), *ctx.weightset()};
    }

    template <typename Context, size_t Tape>
    using blind_context_t = decltype(blind_context<Tape>(std::declval<Context>()));

    /*------------------.
    | blind_automaton.  |
    `------------------*/

    /// Read-write on an automaton, that hides all bands but one.
    template <std::size_t Band, typename Aut>
    class blind_automaton_impl
      : public automaton_decorator<Aut,
                                   blind_context_t<context_t_of<Aut>, Band>>
    {
    public:
      /// The type of the wrapped automaton.
      using automaton_t = Aut;

      static_assert(context_t_of<Aut>::is_lat, "requires labels_are_tuples");
      static_assert(Band < labelset_t_of<Aut>::size(),
                    "band outside of the tuple");

      /// The type of automata to produce this kind of automata.  For
      /// instance, insplitting on a blind_automaton<const
      /// mutable_automaton<Ctx>> should yield a
      /// blind_automaton<mutable_automaton<Ctx>>, without the "inner"
      /// const.
      using automaton_nocv_t
        = blind_automaton<Band,
                          typename automaton_t::element_type::automaton_nocv_t>;

      /// This automaton's state and transition types are those of the
      /// wrapped automaton.
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;

      /// Underlying automaton context.
      using full_context_t = context_t_of<automaton_t>;
      /// Underlying automaton labelset.
      using full_labelset_t = typename full_context_t::labelset_t;
      /// Underlying automaton label.
      using full_label_t = typename full_labelset_t::value_t;

      /// Exposed context.
      using context_t = blind_context_t<full_context_t, Band>;

      /// Exposed labelset.
      using labelset_t = typename context_t::labelset_t;
      using labelset_ptr = typename context_t::labelset_ptr;
      using label_t = typename labelset_t::value_t;

      /// Exposed weightset.
      using weightset_t = typename context_t::weightset_t;
      using weightset_ptr = typename context_t::weightset_ptr;
      using weight_t = typename weightset_t::value_t;

      /// Indices of the remaining tapes.
      using hidden_indices_t
        = concat_sequence
          <typename make_index_range<0, Band>::type,
           typename make_index_range<Band + 1,
                                     std::tuple_size<full_label_t>::value - Band - 1>::type>;

      // All bands except the exposed one.
      using res_labelset_t = typename hidden_label_type<Aut, hidden_indices_t>::type;
      using res_label_t = typename res_labelset_t::value_t;

      using super_t = automaton_decorator<automaton_t, context_t>;

    public:

      blind_automaton_impl(const full_context_t& ctx)
        : blind_automaton_impl(make_shared_ptr<automaton_t>(ctx))
      {}

      blind_automaton_impl(const automaton_t& aut)
        : super_t(aut)
      {}

      static std::string sname()
      {
        return ("blind_automaton<" + std::to_string(Band) + ", "
                + automaton_t::element_type::sname() + ">");
      }

      std::string vname(bool full = true) const
      {
        return ("blind_automaton<" + std::to_string(Band) + ", "
                + aut_->vname(full) + ">");
      }

      full_context_t full_context() const
      {
        return aut_->context();
      }

      context_t context() const
      {
        return context_;
      }

      res_label_t
      hidden_label_of(transition_t t) const
      {
        return hidden_label_of_(t, hidden_indices);
      }

      res_label_t
      hidden_one() const
      {
        return hidden_one_<full_labelset_t>(hidden_indices);
      }

      res_labelset_t
      res_labelset() const
      {
        return res_labelset_(hidden_indices);
      }

      /// Apparent labelset.
      std::shared_ptr<labelset_t>
      labelset() const
      {
        return std::make_shared<labelset_t>(aut_->labelset()->template set<Band>());
      }

    private:
      using super_t::aut_;

      hidden_indices_t hidden_indices{};

      static label_t hide_(full_label_t l)
      {
        return std::get<Band>(l);
      }

      template <std::size_t... I>
      res_label_t hidden_label_of_(transition_t t, index_sequence<I...>) const
      {
        full_label_t l = aut_->label_of(t);
        return std::make_tuple(std::get<I>(l)...);
      }

      template <typename L, std::size_t... I>
      typename std::enable_if<L::has_one(), res_label_t>::type
      hidden_one_(index_sequence<I...>) const
      {
        full_label_t l = aut_->labelset()->one();
        return std::make_tuple(std::get<I>(l)...);
      }

      template <typename L, std::size_t... I>
      typename std::enable_if<!L::has_one(), res_label_t>::type
      hidden_one_(index_sequence<I...>) const
      {
        raise("Should not get here");
      }

      template <std::size_t... I>
      res_labelset_t res_labelset_(index_sequence<I...>) const
      {
        return res_labelset_t{std::get<I>(aut_->labelset()->sets())...};
      }

    public:

      /*----------------------------.
      | const methods that change.  |
      `----------------------------*/

      auto label_of(transition_t t) const
        -> label_t
      {
        return hide_(aut_->label_of(t));
      }

      /// Copy the full wrapped transition.
      template <typename A>
      transition_t
      new_transition_copy(state_t src, state_t dst,
                          const A& aut,
                          typename A::element_type::transition_t t,
                          bool transpose = false)
      {
        return aut_->new_transition_copy(src, dst,
                                         aut->strip(), t, transpose);
      }

      /// Copy the full wrapped transition.
      template <typename A>
      weight_t
      add_transition_copy(state_t src, state_t dst,
                          const A& aut,
                          typename A::element_type::transition_t t,
                          bool transpose = false)
      {
        return aut_->add_transition_copy(src, dst,
                                         aut->strip(), t, transpose);
      }

    private:
      /// Our apparent context.  This is not a luxury to cache it:
      /// benches show that in some cases, intensive (and admittedly
      /// wrong: they should have been cached on the caller side)
      /// calls to context() ruins the performances.
      context_t context_ = blind_context<Band>(full_context());
    };
  }

  template <std::size_t Tape, typename Aut>
  using blind_automaton
    = std::shared_ptr<detail::blind_automaton_impl<Tape, Aut>>;

  template <unsigned Tape, typename Aut>
  inline
  blind_automaton<Tape, Aut>
  blind(Aut aut)
  {
    return std::make_shared<detail::blind_automaton_impl<Tape, Aut>>(aut);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Tape>
      automaton
      blind(automaton& aut, unsigned tape)
      {
        auto& a = aut->as<Aut>();
        // FIXME: we currently have no support for values in parameters.
        if (tape == 0)
          return make_automaton(vcsn::blind<0>(a));
        else
          return make_automaton(vcsn::blind<1>(a));
      }

      REGISTER_DECLARE(blind,
                       (automaton& aut, unsigned tape) -> automaton);
    }
  }

}

#endif // !VCSN_ALGOS_BLIND_HH
