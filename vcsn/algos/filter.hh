#ifndef VCSN_ALGOS_FILTER_HH
# define VCSN_ALGOS_FILTER_HH

# include <algorithm> // std::max
# include <boost/range/irange.hpp>
# include <vcsn/algos/copy.hh>
# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/core/crange.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/unordered_set.hh>
# include <vcsn/misc/vector.hh>

namespace vcsn
{
  namespace detail
  {
    /// Hide some states of an automaton.
    template <typename Aut>
    class filter_automaton_impl
      : public automaton_decorator<Aut>
    {
    public:
      using automaton_t = Aut;
      using super_t = automaton_decorator<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using transition_t = transition_t_of<automaton_t>;
      using label_t = label_t_of<automaton_t>;

      /// States set that we want to filter.
      /// Using std::unordered_set because when test states_t and
      /// accessible states are std::unordered_set the
      /// score(perfomance) is better(x2) than using std::set.
      using states_t = std::unordered_set<state_t>;

      using tr_cont_t = std::vector<transition_t>;
      using states_output_t =
        container_filter_range<boost::integer_range<state_t>>;

      using super_t::pre;
      using super_t::post;
      using super_t::src_of;
      using super_t::dst_of;

      filter_automaton_impl(const automaton_t& input, const states_t& ss)
        : super_t(input), ss_(ss)
      {
        ss_.emplace(input->pre());
        ss_.emplace(input->post());
      }

      /// Static name.
      static std::string sname()
      {
        return "filter_automaton<" + automaton_t::element_type::sname() + ">";
      }

      /// Dynamic name.
      std::string vname(bool full = true) const
      {
        return "filter_automaton<" + aut_->vname(full) + ">";
      }

      bool state_has_name(state_t s) const
      {
        assert(has(ss_, s));
        return aut_->state_has_name(s);
      }

      bool has_state(state_t s) const
      {
        return has(ss_, s) && aut_->has_state(s);
      }

      std::ostream& print_state_name(state_t s, std::ostream& o,
                                     const std::string& fmt = "text",
                                     bool delimit = false) const
      {
        assert(has(ss_, s));
        return aut_->print_state_name(s, o, fmt, delimit);
      }

      size_t num_states()
      {
        return states().size();
      }

      size_t num_all_states()
      {
        return all_states().size();
      }

      template <typename Pred>
      states_output_t all_states(Pred pred) const
      {
        return aut_->all_states([this, pred](state_t s)
                                { return pred(s) && has(ss_, s); });
      }

      states_output_t all_states() const
      {
        return all_states([](state_t) { return true; });
      }


      states_output_t states() const
      {
        return all_states([this](state_t s)
                          {
                            // In the case transposing
                            // pre() > post().
			    return std::max(pre(), post()) < s;
                          });
      }

      template <typename Pred>
      typename super_t::transitions_output_t
      all_transitions(Pred pred) const
      {
        return aut_->all_transitions([this, pred] (transition_t t)
                                     {
                                       return (pred(t)
                                               && has(ss_, src_of(t))
                                               && has(ss_, dst_of(t)));
                                     });
      }

      /// All the transition indexes between all states (including pre
      /// and post).
      typename super_t::transitions_output_t
      all_transitions() const
      {
        return all_transitions([] (transition_t) { return true; });
      }

      /// All the transition indexes between visible states.
      typename super_t::transitions_output_t
      transitions() const
      {
        return all_transitions([this] (transition_t t)
                               {
                                 return (src_of(t) != pre()
                                         && dst_of(t) != post());
                               });
      }

      template <typename Pred>
      container_filter_range<const tr_cont_t&>
      all_out(state_t s, Pred pred) const
      {
        return aut_->all_out(s,
                             [this, pred] (transition_t t)
                             {
                               return pred(t) && has(ss_, dst_of(t));
                             });
      }

      container_filter_range<const tr_cont_t&>
      all_out(state_t s) const
      {
        return all_out(s,
                       [] (transition_t) { return true; });
      }

      container_filter_range<const tr_cont_t&>
      out(state_t s) const
      {
        return all_out(s,
                       [this] (transition_t t)
                       {
                         return dst_of(t) != post();
                       });
      }

      container_filter_range<const tr_cont_t&>
      out(state_t s, const label_t& l) const
      {
        return all_out(s,
                       [this, l] (transition_t t)
                       {
                         return aut_.labelset()->equal(aut_->label_of(t, l));
                       });
      }

      template <typename Pred>
      container_filter_range<const tr_cont_t&>
      all_in(state_t s, Pred pred) const
      {
        return aut_->all_in(s,
                            [this, pred] (transition_t t)
                            {
                              return pred(t) && has(ss_, src_of(t));
                            });
      }

      container_filter_range<const tr_cont_t&>
      all_in(state_t s) const
      {
        return all_in(s, [] (transition_t) { return true; });
      }

      container_filter_range<const tr_cont_t&>
      in(state_t s) const
      {
        return all_in(s,
                      [this] (transition_t t)
                      {
                        return src_of(t) != post();
                      });
      }

      container_filter_range<const tr_cont_t&>
      in(state_t s, const label_t& l) const
      {
        return all_in(s,
                      [this, l] (transition_t t)
                      {
                        return (aut_.labelset()->equal(aut_->label_of(t), l));
                      });
      }

      typename automaton_t::element_type::automaton_nocv_t
      strip() const
      {
        return ::vcsn::copy(aut_, ss_);
      }

      /// Indexes of transitions to visible initial states.
      container_filter_range<const tr_cont_t&>
      initial_transitions() const
      {
        return out(pre());
      }

      /// Indexes of transitions from visible final states.
      container_filter_range<const tr_cont_t&>
      final_transitions() const
      {
        return in(post());
      }

    protected:
      /// The "full" automaton whose some states are hidden.
      using super_t::aut_;

    private:
      /// The states we keep.
      states_t ss_;
   };
  }

  template <typename Aut>
  using filter_automaton =
    std::shared_ptr<detail::filter_automaton_impl<Aut>>;

  /// Get an automaton who is a part state set \a ss of \a aut.
  template <typename Aut>
  inline
  filter_automaton<Aut>
  filter(const Aut& aut, const std::unordered_set<state_t_of<Aut>>& ss)
  {
    return make_shared_ptr<filter_automaton<Aut>>(aut, ss);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Unsigneds>
      automaton
      filter(const automaton& aut, const std::vector<unsigned>& states)
      {
        const auto& a = aut->as<Aut>();
        std::unordered_set<state_t_of<Aut>> ss;
        // FIXME: this is wrong, of course.
        for (auto s: states)
          ss.emplace(s + 2);
        return make_automaton(::vcsn::filter(a, ss));
      }

      REGISTER_DECLARE(filter,
                       (const automaton& aut,
                        const std::vector<unsigned>& ss) -> automaton);
    }
  }
}

#endif //!VCSN_ALGOS_FILTER_HH
