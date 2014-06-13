#ifndef VCSN_ALGOS_DETERMINIZE_HH
# define VCSN_ALGOS_DETERMINIZE_HH

# include <set>
# include <stack>
# include <string>
# include <type_traits>
# include <map>
# include <unordered_map>
# include <vector>

# include <vcsn/core/automaton-decorator.hh>
# include <vcsn/ctx/traits.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/dynamic_bitset.hh>
# include <vcsn/misc/map.hh> // vcsn::has

namespace vcsn
{

  /*----------------------.
  | subset construction.  |
  `----------------------*/
  namespace detail
  {
    /// \brief The subset construction automaton from another.
    ///
    /// \tparam AutPtr an automaton type.
    /// \precondition labelset is free.
    /// \precondition weightset is Boolean.
    template <typename AutPtr>
    class determinized_automaton_impl
      : public automaton_decorator<typename AutPtr::element_type::automaton_nocv_t>
    {
      static_assert(labelset_t_of<AutPtr>::is_free(),
                    "determinize: requires free labelset");
      static_assert(std::is_same<weight_t_of<AutPtr>, bool>::value,
                    "determinize: requires Boolean weights");

    public:
      using automaton_ptr = AutPtr;
      using automaton_t = typename automaton_ptr::element_type;
      using automaton_nocv_t = typename automaton_t::automaton_nocv_t;
      using label_t = label_t_of<automaton_t>;
      using super_t = automaton_decorator<automaton_nocv_t>;

      /// Set of (input) states.
      using state_set = dynamic_bitset;

      /// Result automaton state type.
      using state_t = state_t_of<automaton_t>;

      /// Build the determinizer.
      /// \param a         the automaton to determinize
      /// \param complete  whether to force the result to be complete
      determinized_automaton_impl(const automaton_ptr& a, bool complete = false)
        : super_t(a->context())
        , input_(a)
        , complete_(complete)
        , finals_(state_size_)
      {
        // Input final states.
        for (auto t : input_->final_transitions())
          finals_.set(input_->src_of(t));

        // Cache the successor states per letter.
        for (auto st : input_->all_states())
          for (auto l : letters_)
            {
              state_set& ss = successors_[st][l];
              ss.resize(state_size_);
              for (auto tr : input_->out(st, l))
                ss.set(input_->dst_of(tr));
            }

        // The input initial states.
        state_set next;
        next.resize(state_size_);
        for (auto t : input_->initial_transitions())
          next.set(input_->dst_of(t));
        // Also pushes the initial state in the todo.
        this->set_initial(state(next));
      }

      static std::string sname()
      {
        return "determinized_automaton<" + automaton_t::sname() + ">";
      }

      std::string vname(bool full = true) const
      {
        return "determinized_automaton<" + input_->vname(full) + ">";
      }

      /// The state for set of states \a ss.
      /// If this is a new state, schedule it for visit.
      state_t state(const state_set& ss)
      {
        state_t res;
        auto i = map_.find(ss);
        if (i == std::end(map_))
          {
            res = this->new_state();
            map_[ss] = res;

            if (ss.intersects(finals_))
              this->set_final(res);

            todo_.push(ss);
          }
        else
          res = i->second;
        return res;
      };

      /// Determinize all the states.
      void operator()()
      {
        state_set next;
        next.resize(state_size_);
        while (!todo_.empty())
          {
            auto ss = std::move(todo_.top());
            state_t src = state(ss);
            todo_.pop();
            for (auto l: letters_)
              {
                next.reset();
                for (auto s = ss.find_first(); s != ss.npos;
                     s = ss.find_next(s))
                  next |= successors_[s][l];
                // Don't generate the sink.
                if (complete_ || next.any())
                  this->new_transition(src, state(next), l);
              }
          }
      }

      bool state_has_name(state_t s) const
      {
        return (s != super_t::pre()
                && s != super_t::post()
                && has(origins(), s));
      }

      std::ostream&
      print_state_name(state_t ss, std::ostream& o,
                       const std::string& fmt = "text") const
      {
        auto i = origins().find(ss);
        if (i == std::end(origins()))
          this->print_state(ss, o);
        else
          {
            const char* sep = "";
            for (auto s: i->second)
              {
                o << sep;
                input_->print_state_name(s, o, fmt);
                sep = ", ";
              }
          }
        return o;
      }

      /// A map from determinized states to sets of original states.
      using origins_t = std::map<state_t, std::set<state_t>>;
      mutable origins_t origins_;

      const origins_t&
      origins() const
      {
        if (origins_.empty())
          for (const auto& p: map_)
            {
              std::set<state_t> from;
              const auto& ss = p.first;
              for (auto s = ss.find_first(); s != ss.npos;
                   s = ss.find_next(s))
                from.emplace(s);
              origins_.emplace(p.second, std::move(from));
            }
        return origins_;
      }

    private:
      /// Set of input states -> output state.
      using map = std::unordered_map<state_set, state_t>;
      map map_;

      /// Input automaton.
      automaton_ptr input_;

      /// Whether to build a complete automaton.
      bool complete_ = false;

      /// We use state numbers as indexes, so we need to know the last
      /// state number.  If states were removed, it is not the same as
      /// the number of states.
      size_t state_size_ = input_->all_states().back() + 1;

      /// The sets of (input) states waiting to be processed.
      using stack = std::stack<state_set>;
      stack todo_;

      /// Set of final states in the input automaton.
      state_set finals_;

      /// The generators.
      const typename labelset_t_of<AutPtr>::genset_t& letters_
        = input_->labelset()->genset();

      /// successors[SOURCE-STATE][LABEL] = DEST-STATESET.
      using successors_t = std::vector<std::unordered_map<label_t, state_set>>;
      successors_t successors_{state_size_};
    };
  }

  /// A determinized automaton as a shared pointer.
  template <typename Aut>
  using determinized_automaton
    = std::shared_ptr<detail::determinized_automaton_impl<Aut>>;

  template <typename Aut>
  inline
  auto
  determinize(const Aut& a, bool complete = false)
    -> determinized_automaton<Aut>
  {
    auto res = make_shared_ptr<determinized_automaton<Aut>>(a, complete);
    // Determinize.
    res->operator()();
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Bool>
      automaton
      determinize(const automaton& aut, bool complete)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(determinize(a, complete));
      }

      REGISTER_DECLARE(determinize,
                       (const automaton& aut, bool complete) -> automaton);
    }
  }

} // namespace vcsn

#endif // !VCSN_ALGOS_DETERMINIZE_HH
