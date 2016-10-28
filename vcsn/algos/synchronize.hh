#pragma once

#include <stack>
#include <unordered_map>

#include <vcsn/algos/fwd.hh>
#include <vcsn/algos/letterize.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/pair.hh> // hash
#include <vcsn/misc/unordered_map.hh> // has

namespace vcsn
{
  /*---------------.
  | Function tag.  |
  `---------------*/

  CREATE_FUNCTION_TAG(synchronize);

  namespace detail
  {

    template <Automaton Aut>
    struct worded_automaton
    {
      template <typename Dummy>
      struct worded_labelset {};

      template <typename... LabelSet>
      struct worded_labelset<tupleset<LabelSet...>>
      {
        using in_labelset_t = tupleset<LabelSet...>;
        using labelset_t = tupleset<law_t<LabelSet>...>;
        /// A static list of integers.
        template <std::size_t... I>
        using seq = vcsn::detail::index_sequence<I...>;
        using index_t = detail::make_index_sequence<labelset_t::size()>;

        static labelset_t labelset(const in_labelset_t& ls)
        {
          return labelset_(ls, index_t{});
        }

        template <std::size_t... I>
        static labelset_t labelset_(const in_labelset_t& ls, seq<I...>)
        {
          return labelset_t{(make_wordset(std::get<I>(ls.sets())))...};
        }
      };
      using in_automaton_t = Aut;
      using in_context_t = context_t_of<in_automaton_t>;
      using in_labelset_t = labelset_t_of<in_automaton_t>;

      /// Weightset of the worded automaton (same as input)
      using weightset_t = weightset_t_of<in_automaton_t>;

      /// Labelset of the worded automaton
      using labelset_t = typename worded_labelset<in_labelset_t>::labelset_t;
      /// Context of the worded automaton
      using context_t = context<labelset_t, weightset_t>;
      /// Type of the worded automaton
      using automaton_t = mutable_automaton<context_t>;

      /// Create the worded labelset from the original one
      static labelset_t labelset(const in_labelset_t& ls)
      {
        return worded_labelset<in_labelset_t>::labelset(ls);
      }
    };

    template <Automaton Aut>
    using fresh_worded_automaton_t
      = fresh_automaton_t_of<typename worded_automaton<Aut>::automaton_t>;

    /**
     * An automaton whose states may be qualified by delays and/or prefixes.
     *
     * The semantics of the states are so varied that they cannot be easily and
     * clearly represented.
     */
    template <Automaton Aut>
    class synchronized_automaton_impl
      : public automaton_decorator<fresh_worded_automaton_t<Aut>>
    {
    public:
      using automaton_t = Aut;
      using fresh_t = fresh_worded_automaton_t<Aut>;
      using super_t = automaton_decorator<fresh_t>;
      using context_t = context_t_of<super_t>;
      using labelset_t = labelset_t_of<super_t>;
      using label_t = label_t_of<super_t>;
      using weightset_t = weightset_t_of<super_t>;
      using weight_t = weight_t_of<super_t>;
      using state_t = state_t_of<super_t>;

      /// State + delay
      using state_name_t = std::pair<state_t_of<automaton_t>, label_t>;

      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      synchronized_automaton_impl(const automaton_t& aut)
        : super_t(context_t{worded_automaton<Aut>::labelset(*aut->labelset()),
                            *aut->weightset()})
        , aut_(aut)
      {
        todo_.emplace(aut->pre(), labelset_t::one());
      }

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"synchronized_automaton<"
                          + automaton_t::element_type::sname()
                          + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "synchronized_automaton<";
        super_t::print_set(o, fmt);
        return o << '>';
      }

      /// Symbolic states to state handlers.
      using smap = std::unordered_map<state_name_t, state_t>;

      /// The state for delay \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const state_name_t& r, bool todo = true)
      {
        if (r.first == aut_->pre())
          return this->pre();
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(r);
        if (i == std::end(map_))
          {
            res = super_t::new_state();
            map_[r] = res;
            if (todo)
              todo_.push(r);
          }
        else
          res = i->second;
        return res;
      }

      using super_t::new_transition;

      void
      new_transition(const state_name_t& src, const state_name_t& dst,
                     const label_t& l, const weight_t& w)
      {
        super_t::new_transition(state(src), state(dst), l, w);
      }

      using super_t::set_final;

      void set_final(const state_name_t& st, const weight_t& w)
      {
        super_t::set_final(map_[st], w);
      }

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {},
                       bool = false) const
      {
        auto name = origins().at(s);
        aut_->print_state_name(name.first, o, fmt, true);
        o << ':';
        this->labelset()->print(name.second, o, fmt.for_labels());
        return o;
      }

      /// Ordered map: state -> its delayed state.
      using origins_t = std::unordered_map<state_t, state_name_t>;
      mutable origins_t origins_;

      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_[p.second] = p.first;
        return origins_;
      }

      /// States to visit.
      std::stack<state_name_t, std::vector<state_name_t>> todo_;
      /// delayed_state -> state.
      smap map_;
      /// The original automaton.
      automaton_t aut_;
    };

    template <Automaton Aut>
    using synchronized_automaton
      = std::shared_ptr<synchronized_automaton_impl<Aut>>;

    template <Automaton Aut>
    class synchronizer
    {
      static_assert(context_t_of<Aut>::is_lat,
                    "synchronize: automaton labelset must be a tupleset");

    public:
      using automaton_t = Aut;
      using out_automaton_t = synchronized_automaton<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using labelset_t = labelset_t_of<out_automaton_t>;
      using weightset_t = weightset_t_of<out_automaton_t>;
      using label_t = typename labelset_t::value_t;
      using state_name_t = typename out_automaton_t::element_type::state_name_t;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

      static constexpr size_t number_of_tapes = labelset_t_of<Aut>::size();

      using index_t = detail::make_index_sequence<number_of_tapes>;

      static constexpr index_t indices = {};

      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      synchronizer(const automaton_t& aut)
        : in_aut_(aut), out_aut_(make_shared_ptr<out_automaton_t>(aut))
      {}

      out_automaton_t synchronize()
      {
        while (!out_aut_->todo_.empty())
        {
          state_name_t st = std::move(out_aut_->todo_.top());
          out_aut_->todo_.pop();
          auto s = st.first;
          label_t l = st.second;
          if (in_aut_->is_final(s))
            {
              if (labelset_t::is_one(l))
                out_aut_->set_final(st, in_aut_->get_final_weight(s));
              else
                {
                  auto f = state_name_t{s, labelset_t::one()};
                  // Create the state, don't add it to the todo list.
                  out_aut_->state(f, false);
                  out_aut_->new_transition(st, f, l,
                                           in_aut_->get_final_weight(s));
                  out_aut_->set_final(f, weightset_t::one());
                }
            }

          for (auto tr : out(in_aut_, s))
          {
            label_t combined =
              out_aut_->labelset()->mul(l,
                    out_aut_->labelset()->conv(*in_aut_->labelset(),
                                               in_aut_->label_of(tr)));

            auto pre_suf = get_prefix(combined);
            out_aut_->new_transition(st,
                                     {in_aut_->dst_of(tr), pre_suf.second},
                                     pre_suf.first,
                                     in_aut_->weight_of(tr));
          }
        }
        out_aut_->properties().update(synchronize_ftag{});
        return out_aut_;
      }

    private:

      std::pair<label_t, label_t>
      get_prefix(const label_t& l)
      {
        return get_prefix(get_min_length(l), l);
      }

      /// Split the label in prefix/suffix, with the prefix of size
      /// length.
      std::pair<label_t, label_t>
      get_prefix(size_t length, const label_t& l)
      {
        auto ls = out_aut_->labelset();
        auto prefix = labelset_t::one();
        auto suffix = labelset_t::one();

        size_t i = 0;
        for (auto letter :
             ls->letters_of_padded(l,
                        letterized_traits<labelset_t>::labelset_t::one()))
        {
          if (i < length)
          {
            ++i;
            prefix = ls->mul(prefix, letter);
          }
          else
            suffix = ls->mul(suffix, letter);
        }

        return {prefix, suffix};
      }

      size_t
      get_min_length(const label_t& l)
      {
        return get_min_length_(l, indices);
      }

      template <size_t... I>
      size_t
      get_min_length_(const label_t& l, seq<I...>)
      {
        return std::min({tape_labelset_t<I>::size(std::get<I>(l))...});
      }
      automaton_t in_aut_;
      out_automaton_t out_aut_;
    };

    template <Automaton Aut>
    typename synchronizer<Aut>::out_automaton_t
    synchronize(const Aut& aut)
    {
      synchronizer<Aut> s(aut);
      return s.synchronize();
    }

  }

  /*--------------.
  | synchronize.  |
  `--------------*/

  /// Synchronize the transducer
  ///
  /// \param[in] aut        the transducer
  /// \returns              the synchronized transducer
  template <Automaton Aut>
  auto
  synchronize(const Aut& aut)
    -> decltype(detail::synchronize(aut))
  {
    return detail::synchronize(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton synchronize(const automaton& aut)
      {
        return ::vcsn::synchronize(aut->as<Aut>());
      }
    }
  }
}
