#pragma once

#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <boost/optional/optional.hpp>

#include <vcsn/algos/fwd.hh>
#include <vcsn/algos/copy.hh>
#include <vcsn/algos/letterize.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/misc/pair.hh> // hash

namespace vcsn
{

  namespace detail
  {

    template <typename Labelset>
    struct special_remover
    {
      using labelset_t = Labelset;
      using label_t = typename labelset_t::value_t;

      static label_t remove(label_t l)
      {
        return labelset_t::is_special(l)
               ? labelset_t::one()
               : l;
      }
    };

    template <typename Labelset>
    struct special_remover<nullableset<Labelset>>
    {
      using labelset_t = nullableset<Labelset>;
      using label_t = typename labelset_t::value_t;

      static label_t remove(label_t l)
      {
        return labelset_t::is_special(l)
               ? labelset_t::one()
               : special_remover<Labelset>::remove(l);
      }
    };

    template <typename GenSet>
    struct special_remover<wordset<GenSet>>
    {
      using labelset_t = wordset<GenSet>;
      using label_t = typename labelset_t::value_t;
      using letterset_t = letterset<GenSet>;

      static label_t remove(label_t label)
      {
        label_t res;
        for (auto l : label)
          if (!letterset_t::is_special(l))
            res += l;
        return res;
      }
    };

    template <typename... Labelsets>
    struct special_remover<tupleset<Labelsets...>>
    {
      using labelset_t = tupleset<Labelsets...>;
      using label_t = typename labelset_t::value_t;

      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;
      using index_t = detail::make_index_sequence<labelset_t::size()>;

      static label_t remove(label_t l)
      {
        return remove_(l, index_t{});
      }

      template <size_t... I>
      static label_t remove_(label_t l, seq<I...>)
      {
        return label_t{(tape_labelset_t<I>::is_special(std::get<I>(l))
                       ? tape_labelset_t<I>::one()
                       : special_remover<tape_labelset_t<I>>::remove(std::get<I>(l)))... };
      }
    };

    // Wrapper function to remove special letters
    template <typename LabelSet>
    typename LabelSet::value_t
    remove_special(const typename LabelSet::value_t& l)
    {
        return special_remover<LabelSet>::remove(l);
    }

    template <typename Aut>
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

    template <typename Aut>
    using fresh_worded_automaton_t = fresh_automaton_t_of<typename worded_automaton<Aut>::automaton_t>;

    /**
     * An automaton whose states may be qualified by delays and/or prefixes.
     *
     * The semantics of the states are so varied that they cannot be easily and
     * clearly represented.
     */
    template <typename Aut>
    class delay_automaton_impl
      : public automaton_decorator<fresh_worded_automaton_t<Aut>>
    {
    public:
      using automaton_t = Aut;
      using return_t = fresh_worded_automaton_t<Aut>;
      using super_t = automaton_decorator<return_t>;
      using context_t = context_t_of<super_t>;
      using labelset_t = labelset_t_of<super_t>;
      using label_t = label_t_of<super_t>;
      using weightset_t = weightset_t_of<super_t>;
      using weight_t = weight_t_of<super_t>;
      using state_t = state_t_of<super_t>;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;

      static constexpr size_t number_of_tapes = labelset_t_of<Aut>::size();

      using index_t = detail::make_index_sequence<number_of_tapes>;

      static constexpr index_t indices = {};

      /// The delay associated with each state
      using delay_t = std::array<size_t, number_of_tapes>;

      /// State + delay
      using state_name_t = std::pair<state_t, delay_t>;
      using opt_state_t = boost::optional<state_name_t>;


      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      delay_automaton_impl(const automaton_t& aut)
        : super_t(context_t{worded_automaton<Aut>::labelset(*aut->labelset()), *aut->weightset()})
        , aut_(aut)
      {
        map_[state_name_t(this->pre(), delay_t{})] = aut->pre();
        map_[state_name_t(this->post(), delay_t{})] = aut->post();
      }

      /// Static name.
      static symbol sname()
      {
        static symbol res("delay_automaton<" + automaton_t::element_type::sname() + '>');
        return res;
      }

      std::ostream& print_set(std::ostream& o, const std::string& format) const
      {
        o << "delay_automaton<";
        super_t::print_set(o, format);
        return o << '>';
      }

      /// Symbolic states to state handlers.
      using smap = std::unordered_map<state_name_t, state_t>;

      /// The state for delay \a r.
      /// If this is a new state, schedule it for visit.
      state_t state(const state_name_t& r)
      {
        // Benches show that the map_.emplace technique is slower, and
        // then that operator[] is faster than emplace.
        state_t res;
        auto i = map_.find(r);
        if (i == std::end(map_))
          {
            res = super_t::new_state();
            map_[r] = res;
            todo_.push(r);
            if (r.second != delay_t{})
              selectable_.emplace(r);
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
        if (0 < std::get<1>(src.second) && 0 < std::get<0>(dst.second))
        {
          auto ls = super_t::labelset();
          state_t inter = super_t::new_state();
          auto pre_suf = get_prefix_tape<0>(std::get<1>(src.second), l);
          super_t::new_transition(map_[src], inter, pre_suf.first, w);
          new_transition_(inter, dst, pre_suf.second, weightset_t::one());
        }
        else
          new_transition_(map_[src], dst, l, w);
      }

      // Replace the Jth tape of label with the Jth tape of repl
      template <size_t J, size_t... I>
      label_t
      replace_tape(const label_t& label, const label_t& repl, seq<I...>)
      {
        return label_t{(I == J ? std::get<I>(repl) : std::get<I>(label))...};
      }

      /*
       * Split the Ith tape in prefix/suffix, and returns:
       * - The empty word with the Ith tape filled with the prefix
       * - The original label with Ith tape replaced with the suffix.
       */
      template <size_t I>
      std::pair<label_t, label_t>
      get_prefix_tape(size_t length, const label_t& l)
      {
        auto ls = this->labelset();
        auto prefix = labelset_t::one();
        auto suffix = labelset_t::one();

        size_t i = 0;
        for (auto letter :
             ls->letters_of_padded(l,
                        letterized_labelset<labelset_t>::labelset_t::one()))
        {
          if (i < length)
          {
            ++i;
            prefix = ls->mul(prefix, letter);
          }
          else
            suffix = ls->mul(suffix, letter);
        }

        return {replace_tape<I>(labelset_t::one(), prefix, indices), replace_tape<I>(l, suffix, indices)};
      }

      /* Create a label with each tape being one, except for the indexth, which
       * is special.
       */
      template <size_t... I>
      typename labelset_t::letter_t
      create_fake_special(int index, seq<I...>)
      {
        auto ls = super_t::labelset();
        return typename labelset_t::letter_t
            {(index != I
            ? std::get<I>(ls->sets()).genset().special()
            : std::get<I>(ls->sets()).genset().one_letter())...};
      }

      void
      new_transition_(const state_t& src, const state_name_t& dst,
                      label_t l, const weight_t& w)
      {
        if (dst.first == super_t::post() && dst.second != delay_t{})
        {
          auto ls = super_t::labelset();
          state_t inter = super_t::new_state();
          label_t padding = labelset_t::one();
          auto max = std::max_element(begin(dst.second), end(dst.second));
          int index_max = max - begin(dst.second);
          auto letter = create_fake_special(index_max, indices);
          for (size_t i = 0; i < *max; ++i)
            padding = ls->mul(padding, letter);
          super_t::new_transition(src, inter, padding, w);
          super_t::set_final(inter, weightset_t::one());
        }
        else
          super_t::new_transition(src, state(dst), l, w);
      }

      bool state_has_name(state_t s) const
      {
        // The meaning of the different types of states is too varied to be
        // printed
        return false;
      }

      // Shouldn't be called
      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       const std::string& format = "text",
                       bool delimit = false) const
      {
        /*auto i = origins().find(s);
        if (i == std::end(origins()))
          this->print_state(s, o);
        else
        {
          if (delimit)
            o << '(';
          aut_->print_state_name(i->second.first, o, format, true);
          o << ":(";
          auto a = i->second.second;
          for (int i = 0; i < a.size() - 1; i++)
            o << a[i] << ',';
          if (a.size())
            o << a[a.size() - 1];
          o << ')';
          if (delimit)
            o << ')';
        }*/
        return o;
      }

      delay_t delay_of(state_t s)
      {
        auto i = origins().find(s);
        if (i == std::end(origins()))
          return {};
        else
          return i->second.second;
      }

      /// Ordered map: state -> its delayed state.
      using origins_t = std::map<state_t, state_name_t>;
      mutable origins_t origins_;

      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            origins_[p.second] = p.first;
        return origins_;
      }

      /// Get a selectable state (when considering tape I)
      template <size_t I>
      opt_state_t
      select_state()
      {
        using ls_t = typename labelset_t::template valueset_t<I>;
        for (auto st : selectable_)
        {
          //TODO: change when switching to k-tape (min?)
          if (int delay = std::get<1 - I>(st.second))
          {
            auto trs = super_t::out(map_[st]);
            auto it = std::find_if(begin(trs), end(trs),
                 [this, delay](auto tr)
                 {
                   return ls_t::size(std::get<I>(this->label_of(tr))) >= delay;
                 });
            if (it != end(trs))
            {
              selectable_.erase(st);
              return st;
            }
          }
        }
        return {};
      }

      /// States to visit.
      std::stack<state_name_t, std::vector<state_name_t>> todo_;
      /// delayed_state -> state.
      smap map_;
      /// The original automaton
      const automaton_t& aut_;
      /// States to potentially select
      std::unordered_set<state_name_t> selectable_;
    };

    template <typename Aut>
    using delay_automaton = std::shared_ptr<delay_automaton_impl<Aut>>;

    template <typename Aut>
    class synchronizer
    {
      static_assert(context_t_of<Aut>::is_lat,
                    "synchronize: automaton labelset must be a tupleset");
      // The algorithm currently only makes sense for a 2-tape transducer
      static_assert(labelset_t_of<Aut>::size() == 2,
                    "synchronize: transducer labelset must have exactly 2 tapes");

    public:
      using automaton_t = Aut;
      using out_automaton_t = delay_automaton<automaton_t>;
      using return_automaton_t = typename out_automaton_t::element_type::return_t;
      using state_t = state_t_of<automaton_t>;
      using in_labelset_t = labelset_t_of<automaton_t>;
      using in_label_t = typename in_labelset_t::value_t;
      using labelset_t = labelset_t_of<out_automaton_t>;
      using label_t = typename labelset_t::value_t;
      using delay_t = typename out_automaton_t::element_type::delay_t;
      using opt_state_t = typename out_automaton_t::element_type::opt_state_t;
      using state_name_t = typename out_automaton_t::element_type::state_name_t;

      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      template <size_t I>
      using in_tape_labelset_t = typename in_labelset_t::template valueset_t<I>;

      template <size_t I>
      using tape_label_t = typename tape_labelset_t<I>::value_t;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;


      // map state_t -> delay
      using delay_map_t = std::vector<delay_t>;

      // A state with the associated delay
      using value_state_t = std::pair<state_t, delay_t>;
      // map state_with_delay -> actual state number
      using value_map_t = std::unordered_map<value_state_t, state_t>;

      synchronizer(const automaton_t& aut)
        : in_aut_(aut), out_aut_(make_shared_ptr<out_automaton_t>(aut))
      {}


      return_automaton_t synchronize()
      {
        // tag the states with the delays
        value_automaton();
        // synchronize
        synchronize_(out_aut_->indices);
        // Remove the special
        for (auto st : out_aut_->states())
          for (auto tr : out_aut_->out(st))
          {
            auto src = out_aut_->src_of(tr);
            auto dst = out_aut_->dst_of(tr);
            auto l = out_aut_->label_of(tr);
            auto w = out_aut_->weight_of(tr);
            out_aut_->del_transition(tr);
            out_aut_->new_transition(src, dst,
                                     remove_special<labelset_t>(l),
                                     w);
          }
        // Decorator is useless
        return out_aut_->strip();
      }

    private:

      void value_automaton()
      {
        out_aut_->todo_.emplace(in_aut_->pre(), delay_t{});

        while (!out_aut_->todo_.empty())
        {
          auto val_state = std::move(out_aut_->todo_.top());
          auto st = val_state.first;
          delay_t delay = val_state.second;
          out_aut_->todo_.pop();
          for (auto t : in_aut_->all_out(st))
          {
            auto l = in_aut_->label_of(t);
            auto dst = in_aut_->dst_of(t);
            delay_t d = add_delay_(delay, l, out_aut_->indices);
            value_state_t new_state(dst, d);
            out_aut_->new_transition(val_state,
                                     new_state,
                                     out_aut_->labelset()->conv(*(in_aut_->labelset()), l),
                                     in_aut_->weight_of(t));
          }
        }
      }

      /*
       * For a state with delay k on the Ith tape, take the k first letters of
       * the outgoing transitions and add them to the incoming transitions.
       * This can create more than one state.
       */
      template <size_t I>
      void apply_circulation(state_name_t st)
      {
        std::unordered_map<label_t, state_t> prefixes;
        state_t s = out_aut_->map_[st];
        for (auto tr : out_aut_->out(s))
        {
          auto u = out_aut_->template get_prefix_tape<I>(std::get<1 - I>(st.second), out_aut_->label_of(tr)); //FIXME for k
          state_t src;
          auto it = prefixes.find(u.first);
          if (it == end(prefixes))
          {
            src = out_aut_->new_state();
            prefixes[u.first] =  src;
          }
          else
            src = it->second;
          state_t dst = out_aut_->dst_of(tr);
          if (s != dst)
            out_aut_->new_transition(src, dst, u.second, out_aut_->weight_of(tr));
        }

        for (auto tr : out_aut_->in(s))
        {
          state_t src = out_aut_->src_of(tr);
          if (src != s)
            for (const auto& p : prefixes)
              out_aut_->new_transition(src, p.second,
                                       out_aut_->labelset()->mul(out_aut_->label_of(tr), p.first),
                                       out_aut_->weight_of(tr));
          else
          {
            auto u = out_aut_->template get_prefix_tape<I>(std::get<1 - I>(st.second), out_aut_->label_of(tr)); //FIXME for k
            for (const auto& p : prefixes)
              out_aut_->new_transition(prefixes[u.first], p.second,
                                       out_aut_->labelset()->mul(u.second, p.first),
                                       out_aut_->weight_of(tr));
          }
        }
      }

      template <size_t... I>
      void synchronize_(seq<I...>)
      {
        int swallow [] = {(synchronize_<I>(), 0)...};
      }

      template <size_t I>
      void synchronize_()
      {
        while (opt_state_t st = out_aut_->template select_state<I>())
        {
          auto p = *st;
          apply_circulation<I>(p);
          out_aut_->del_state(out_aut_->map_[p]);
        }
      }

      // Add the delay of label l to d
      template <size_t... I>
      delay_t
      add_delay_(delay_t d, const in_label_t& l, seq<I...>) const
      {
        delay_t del = {(std::get<I>(d) + in_tape_labelset_t<I>::size(std::get<I>(l)))...};
        size_t min = *std::min_element(begin(del), end(del));
        return {(std::get<I>(del) - min)...};
      }

      const automaton_t& in_aut_;
      out_automaton_t out_aut_;
      delay_map_t delay_;
      value_map_t value_map_;
    };

    template <typename Aut>
    typename synchronizer<Aut>::return_automaton_t
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
  template <typename Aut>
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
      template <typename Aut>
      automaton synchronize(const automaton& aut)
      {
        return make_automaton(::vcsn::synchronize(aut->as<Aut>()));
      }
    }
  }
}

