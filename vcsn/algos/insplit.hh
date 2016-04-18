#pragma once

#include <unordered_map>

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>

#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/algos/copy.hh> // real_context
#include <vcsn/algos/fwd.hh>
#include <vcsn/misc/bimap.hh>
#include <vcsn/misc/pair.hh>
#include <vcsn/misc/memory.hh>

namespace vcsn
{

  namespace detail
  {
    /// Insplit automaton decorator.
    ///
    /// Build on-the-fly an insplit automaton, i.e., an automaton
    /// whose states have either only proper incoming transitions, or
    /// only spontaneous incoming transitions.
    ///
    /// This is the case where the automaton can have spontaneous
    /// transitions.
    template <Automaton Aut,
              bool HasOne = labelset_t_of<Aut>::has_one()>
    class insplit_automaton_impl
      : public automaton_decorator<fresh_automaton_t_of<Aut>>
    {
      static_assert(labelset_t_of<Aut>::has_one(),
                    "insplit: the labelset must have a one label");

    public:
      using automaton_t = Aut;
      using out_automaton_t = fresh_automaton_t_of<Aut>;

      using self_t = insplit_automaton_impl;

      using super_t = automaton_decorator<out_automaton_t>;

      using state_t = typename super_t::state_t;
      using label_t = typename super_t::label_t;
      using transition_t = typename super_t::transition_t;
      using weightset_t = weightset_t_of<Aut>;
      /// Tuple of states of input automata.
      using state_name_t = std::pair<state_t, bool>;

      using super_t::aut_;

      using bimap_t
        = boost::bimap<boost::bimaps::unordered_set_of<state_name_t>,
                       boost::bimaps::unordered_set_of<state_t>>;
      using map_t = typename bimap_t::left_map;
      using origins_t = typename bimap_t::right_map;


      static symbol sname()
      {
        static symbol res("insplit_automaton<"
                          + Aut::element_type::sname()
                          + ">");
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "insplit_automaton<";
        return aut_->print_set(o, fmt) << ">";
      }

      insplit_automaton_impl(const Aut& aut)
        : super_t(make_fresh_automaton(aut))
        , in_(aut)
      {}

      static constexpr bool
      state_has_name(state_t)
      {
        return true;
      }

      /// Insplit the automaton.
      void insplit(bool lazy = false)
      {
        initialize_insplit_();

        if (!lazy)
          while (!todo_.empty())
            {
              const auto& p = todo_.front();
              this->complete_(std::get<1>(p));
              todo_.pop_front();
            }
      }

      std::ostream&
      print_state_name(state_t s, std::ostream& o,
                       format fmt = {}, bool delimit = false)
                      const
      {
        const auto& orig = origins();
        auto i = orig.find(s);
        if (i == orig.end())
          this->print_state(s, o);
        else
          {
            if (delimit)
              o << '(';
            aut_in()->print_state_name(i->second.first, o, fmt, true);
            o << ", ";
            if (fmt == format::latex)
              o << (i->second.second ? "" : "\\not ") << "\\varepsilon";
            else if (fmt == format::utf8)
              o << (i->second.second ? "ε" : "!ε");
            else
              o << (i->second.second ? "\\e" : "!\\e");
            if (delimit)
              o << ')';
          }
        return o;
      }

      // Lazy

      /// Whether a given state's outgoing transitions have been
      /// computed.
      bool is_lazy(state_t s) const
      {
        return !has(done_, s);
      }

      /// Complete a lazy state: find its outgoing transitions.
      void complete_(state_t s) const
      {
        const auto& orig = origins();
        const state_name_t& sn = orig.at(s);
        const_cast<self_t&>(*this).add_insplit_transitions_(s, sn);
        done_.insert(s);
      }

      /// All the outgoing transitions.
      using super_t::all_out;
      auto all_out(state_t s) const
      {
        if (is_lazy(s))
          complete_(s);
        return aut_->all_out(s);
      }

      // FIXME: make private
      out_automaton_t& aut_out()
      {
        return aut_;
      }

      // FIXME: return shared_ptr to const automaton_impl?
      const out_automaton_t& aut_out() const
      {
        return aut_;
      }

      /// A map from result state to original state and status (spontaneous or
      /// proper state).
      const origins_t& origins() const
      {
        return bimap_.right;
      }


    private:
      /// The input automaton.
      const automaton_t& aut_in() const
      {
        return in_;
      }

      void initialize_insplit_()
      {
        pmap_().insert({state_name_t(aut_in()->pre(), false),
                        aut_out()->pre()});
        pmap_().insert({state_name_t(aut_in()->post(), false),
                        aut_out()->post()});
        todo_.emplace_back(pre_(), aut_->pre());
      }

      state_name_t pre_() const
      {
        return {aut_->pre(), false};
      }

      /// Split the original outgoing transitions to the insplit
      /// states.
      void add_insplit_transitions_(const state_t s,
                                    const state_name_t& sn)
      {
        for (auto t : aut_in()->all_out(std::get<0>(sn)))
          aut_out()->new_transition_copy(s,
                                         state({aut_in()->dst_of(t),
                                               is_spontaneous_(t)}),
                                         aut_in(), t);
      }

      /// Whether transition \a t is labeled by one.
      bool is_spontaneous_(transition_t t) const
      {
        return aut_in()->labelset()->is_one(aut_in()->label_of(t));
      }

      /// The state in the insplit corresponding to a state and a status
      /// (spontaneous or proper state).
      ///
      /// Add the given two source-automaton states to the worklist
      /// for the given result automaton if they aren't already there,
      /// updating the map; in any case return.
      state_t state(const state_name_t& sn)
      {
        auto lb = pmap_().find(sn);
        if (lb == pmap_().end())
          {
            state_t s = aut_->new_state();
            lb = pmap_().insert(lb, {sn, s});
            todo_.emplace_back(sn, s);
          }
        return lb->second;
      }

      /// A map from original state and status (spontaneous or proper state)
      /// to result state.
      map_t& pmap_()
      {
        return bimap_.left;
      }

      /// The input automaton.
      automaton_t in_;

      /// Map input-state, status -> result-state.
      /// status == false: no spontaneous incoming transition
      /// status == true: only spontaneous incoming transitions
      mutable bimap_t bimap_;

      /// When performing the lazy construction, list of states that
      /// have been completed (i.e., their outgoing transitions have
      /// been computed).
      mutable std::set<state_t> done_ = {aut_->post()};

      /// Worklist of state tuples.
      std::deque<std::pair<state_name_t, state_t>> todo_;
    };


    /// Insplit automaton decorator.
    ///
    /// This is the case where the automaton cannot have spontaneous
    /// transitions, so do nothing.
    template <Automaton Aut>
    class insplit_automaton_impl<Aut, false>
      : public automaton_decorator<Aut>
    {
      using super_t = automaton_decorator<Aut>;
    public:
      insplit_automaton_impl(const Aut& aut)
        : super_t(aut)
      {}

      static symbol sname()
      {
        static symbol res("insplit_automaton<"
                          + Aut::element_type::sname()
                          + ">");
        return res;
      }

      using automaton_t = Aut;

      using super_t::aut_;

      void insplit(bool = false)
      {}

      automaton_t& aut_out()
      {
        return aut_;
      }

      const automaton_t& aut_out() const
      {
        return aut_;
      }
    };

    /// An insplit automaton as a shared pointer.
    template <Automaton Aut>
    using insplit_automaton
      = std::shared_ptr<insplit_automaton_impl<Aut>>;

    /// Build an insplit automaton from an automaton.
    template <Automaton Aut>
    auto
    make_insplit_automaton(const Aut& aut)
      -> insplit_automaton<Aut>
    {
      return make_shared_ptr<insplit_automaton<Aut>>(aut);
    }


    /// Insplit an automaton with possible spontaneous transitions.
    template <Automaton Aut>
    auto
    insplit(Aut& aut)
      -> std::enable_if_t<labelset_t_of<Aut>::has_one(),
                          decltype(make_insplit_automaton(aut))>
    {
      auto res = make_insplit_automaton(aut);
      res->insplit(false);
      return res;
    }

    /// Insplit an automaton without spontaneous transitions.
    ///
    /// Identity.
    template <Automaton Aut>
    auto
    insplit(Aut& aut)
      -> std::enable_if_t<!labelset_t_of<Aut>::has_one(),
                          Aut>
    {
      return aut;
    }
  } // namespace detail


  /// An insplit automaton as a shared pointer.
  template <Automaton Aut>
  using insplit_automaton = detail::insplit_automaton<Aut>;

  /// Build a lazy insplit automaton from an automaton.
  template <Automaton Aut>
  auto
  insplit_lazy(const Aut& aut)
  {
    auto res = detail::make_insplit_automaton(aut);
    res->insplit(true);
    return res;
  }


  /// Insplit automaton from an automaton.
  template <Automaton Aut>
  auto
  insplit(const Aut& aut)
  {
    return detail::insplit(aut);
  }


  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <Automaton Aut, typename Bool>
      automaton
      insplit(const automaton& aut, bool lazy)
      {
        const auto& a = aut->as<Aut>();
        if (lazy)
          return ::vcsn::insplit_lazy(a);
        else
          return ::vcsn::insplit(a);
      }
    }
  }

} // namespace vcsn
