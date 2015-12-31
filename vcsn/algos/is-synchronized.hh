#pragma once

#include <stack>
#include <vector>
#include <unordered_map>

#include <vcsn/algos/fwd.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/core/automaton-decorator.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/misc/pair.hh> // hash
#include <vcsn/misc/tuple.hh> // index_sequence
#include <vcsn/misc/unordered_map.hh> // has

namespace vcsn
{
  namespace detail
  {

    /**
     * An automaton whose states may be qualified by delays and/or prefixes.
     *
     * The semantics of the states are so varied that they cannot be easily and
     * clearly represented.
     */
    template <Automaton Aut>
    class delay_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
    {
    public:
      using automaton_t = Aut;
      using super_t = automaton_decorator<fresh_automaton_t_of<automaton_t>>;
      using context_t = context_t_of<super_t>;
      using labelset_t = labelset_t_of<super_t>;
      using label_t = label_t_of<super_t>;
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


      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      delay_automaton_impl(const automaton_t& aut)
        : super_t(aut->context())
        , aut_(aut)
      {
        map_[state_name_t(this->pre(), delay_t{})] = aut->pre();
        map_[state_name_t(this->post(), delay_t{})] = aut->post();
      }

      /// Static name.
      static symbol sname()
      {
        static auto res = symbol{"delay_automaton<"
                          + automaton_t::element_type::sname() + '>'};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "delay_automaton<";
        super_t::print_set(o, fmt);
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
        if (r.first == aut_->post())
          return this->post();
        state_t res;
        auto i = map_.find(r);
        if (i == std::end(map_))
          {
            res = super_t::new_state();
            map_[r] = res;
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

      bool state_has_name(state_t s) const
      {
        return has(origins(), s);
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {},
                       bool = false) const
      {
        auto ns = origins().at(s);
        aut_->print_state_name(ns.first, o, fmt, true);
        o << ":(";
        auto a = ns.second;
        for (int i = 0; i < a.size() - 1; i++)
          o << a[i] << ',';
        if (a.size())
          o << a[a.size() - 1];
        o << ')';
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
      /// The original automaton
      automaton_t aut_;
    };

    template <Automaton Aut>
    class synchronize_checker
    {
      static_assert(context_t_of<Aut>::is_lat,
                    "synchronize: automaton labelset must be a tupleset");

    public:
      using automaton_t = Aut;
      using out_automaton_t = delay_automaton<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using labelset_t = labelset_t_of<out_automaton_t>;
      using label_t = typename labelset_t::value_t;
      using delay_t = typename out_automaton_t::element_type::delay_t;
      using state_name_t = typename out_automaton_t::element_type::state_name_t;

      template <size_t I>
      using tape_labelset_t = typename labelset_t::template valueset_t<I>;

      /// A static list of integers.
      template <std::size_t... I>
      using seq = vcsn::detail::index_sequence<I...>;


      synchronize_checker(const automaton_t& aut)
        : in_aut_(aut), out_aut_(make_shared_ptr<out_automaton_t>(aut))
      {}

      /**
       * Whether the transducer is synchronized.
       *
       * In a synchronized transducer, every state has delay 0, except for some
       * series of states leading to a final state with ever-increasing
       * (strictly except for one-transitions) delay.
       */
      bool is_synchronized()
      {
        // tag the states with the delays
        value_automaton();
        for (auto s : out_aut_->states())
        {
          delay_t d = out_aut_->delay_of(s);
          if (d != delay_t{})
            for (auto tr : out(out_aut_, s))
            {
              if (out_aut_->labelset()->is_one(out_aut_->label_of(tr)))
                continue;
              auto dst = out_aut_->dst_of(tr);
              if (out_aut_->post() == dst)
                continue;
              delay_t dst_d = out_aut_->delay_of(dst);
              for (size_t i = 0; i < out_aut_->number_of_tapes; i++) {
                if (d[i] && dst_d[i] <= d[i])
                  return false;
              }
            }
        }
        return true;
      }

      out_automaton_t
      make_delay_automaton()
      {
        value_automaton();
        return out_aut_;
      }

    private:

      /*
       * Compute the automaton with states tagged with their delays.
       *
       * If split, create the artificial states required for synchronizing.
       */
      void value_automaton()
      {
        out_aut_->todo_.emplace(in_aut_->pre(), delay_t{});

        while (!out_aut_->todo_.empty())
        {
          auto val_state = std::move(out_aut_->todo_.top());
          auto st = val_state.first;
          delay_t delay = val_state.second;
          out_aut_->todo_.pop();
          for (auto t : all_out(in_aut_, st))
          {
            auto l = in_aut_->label_of(t);
            auto dst = in_aut_->dst_of(t);
            delay_t d = add_delay_(delay, l, out_aut_->indices);
            state_name_t new_state(dst, d);
            out_aut_->new_transition(val_state,
                                     new_state,
                                     l,
                                     in_aut_->weight_of(t));
          }
        }
      }

      /// Add the delay of label l to d.
      template <size_t... I>
      delay_t
      add_delay_(delay_t d, const label_t& l, seq<I...>) const
      {
        delay_t del
          = {(std::get<I>(d) + tape_labelset_t<I>::size(std::get<I>(l)))...};
        size_t min = *std::min_element(begin(del), end(del));
        return {(std::get<I>(del) - min)...};
      }

      automaton_t in_aut_;
      out_automaton_t out_aut_;
    };

    template <Automaton Aut>
    bool is_synchronized(const Aut& aut)
    {
      synchronize_checker<Aut> s(aut);
      return s.is_synchronized();
    }

    template <Automaton Aut>
    delay_automaton<Aut>
    make_delay_automaton(const Aut& aut)
    {
      synchronize_checker<Aut> s(aut);
      return s.make_delay_automaton();
    }

  }

  /*------------------.
  | is_synchronized.  |
  `------------------*/

  /// Check whether the transducer is synchronized
  ///
  /// \param[in] aut        the transducer
  /// \returns              whether it is synchronized
  template <Automaton Aut>
  bool
  is_synchronized(const Aut& aut)
  {
    return detail::is_synchronized(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      bool is_synchronized(const automaton& aut)
      {
        return vcsn::is_synchronized(aut->as<Aut>());
      }
    }
  }

  /*------------------.
  | delay_automaton.  |
  `------------------*/

  /// Check whether the transducer is synchronized
  ///
  /// \param[in] aut        the transducer
  /// \returns              whether it is synchronized
  template <Automaton Aut>
  auto
  make_delay_automaton(const Aut& aut)
    -> decltype(detail::make_delay_automaton(aut))
  {
    return detail::make_delay_automaton(aut);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut>
      automaton delay_automaton(const automaton& aut)
      {
        return make_automaton(vcsn::make_delay_automaton(aut->as<Aut>()));
      }
    }
  }
}
