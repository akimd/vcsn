#ifndef VCSN_ALGOS_EDIT_AUTOMATON_HH
# define VCSN_ALGOS_EDIT_AUTOMATON_HH

# include <tuple>
# include <unordered_map>
# include <utility>
# include <vector>

# include <boost/flyweight.hpp>
# include <boost/flyweight/no_tracking.hpp>

# include <vcsn/ctx/ctx.hh>
# include <vcsn/dyn/algos.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/weights/entryset.hh>

namespace std
{
  template <typename T>
  struct hash<boost::flyweight<T, boost::flyweights::no_tracking>>
  {
    using value_type = boost::flyweight<T, boost::flyweights::no_tracking>;
    size_t operator()(const value_type& ss) const
    {
      // http://lists.boost.org/boost-users/2013/03/78007.php
      hash<const void*> hasher;
      return hasher(&ss.get());
    }
  };
}

namespace vcsn
{

  /*----------------------.
  | edit_automaton<Aut>.  |
  `----------------------*/

  /// Abstract Builder (the design pattern) for automata.
  class automaton_editor
  {
  public:
    using string_t =
      boost::flyweight<std::string, boost::flyweights::no_tracking>;

    virtual ~automaton_editor() {}
    virtual void add_initial(const string_t& s, const string_t& w) = 0;
    virtual void add_final(const string_t& s, const string_t& w) = 0;

    virtual void add_state(const string_t& s) = 0;

    /// Declare that \a s denotes the preinitial state in entries.
    virtual void add_pre(const string_t& s) = 0;

    /// Declare that \a s denotes the postfinal state in entries.
    virtual void add_post(const string_t& s) = 0;

    /// Add an entry from \a src to \a dst, with value \a entry.
    virtual void add_entry(const string_t& src, const string_t& dst,
                           const string_t& entry) = 0;
    /// The final result.
    virtual dyn::detail::abstract_automaton* result() = 0;
    /// Forget about the current automaton, but do not free it.
    virtual void reset() = 0;

    /// Set the label separator.  Defaults to '+'.
    void set_separator(char c)
    {
      sep_ = c;
    }

  protected:
    /// The label separator.
    char sep_ = '+';
  };


  /// Concrete Builder (the design pattern) for automata.
  ///
  /// Returns (via result()) an allocated automaton.
  ///
  /// FIXME: cannot be reused for several automata.
  template <typename Aut>
  class edit_automaton: public automaton_editor
  {
  public:
    using super_type = automaton_editor;
    using automaton_t = Aut;
    using string_t = super_type::string_t;

  private:
    using context_t = typename automaton_t::context_t;
    using entry_t = typename entryset<context_t>::value_t;
    using state_t = typename automaton_t::state_t;

    using state_map = std::unordered_map<string_t, state_t>;
    using entry_map = std::unordered_map<string_t, entry_t>;

  public:
    edit_automaton(const context_t& ctx)
      : res_(new automaton_t(ctx))
      , entryset_(ctx)
    {}

    ~edit_automaton()
    {
      delete res_;
    }

    /// Register the existence of state named \a s.
    virtual void
    add_state(const string_t& s) override final
    {
      state_(s);
    }

    /// Register that state named \a s is preinitial.
    virtual void
    add_pre(const string_t& s) override final
    {
      smap_.emplace(s, res_->pre());
    }

    /// Register that state named \a s is postfinal.
    virtual void
    add_post(const string_t& s) override final
    {
      smap_.emplace(s, res_->post());
    }

    virtual void
    add_initial(const string_t& s, const string_t& weight = {}) override final
    {
      auto w = (weight.get().empty() ? res_->weightset()->one()
                : res_->weightset()->conv(weight));
      res_->add_initial(state_(s), w);
    }

    virtual void
    add_final(const string_t& s, const string_t& weight = {}) override final
    {
      auto w = (weight.get().empty() ? res_->weightset()->one()
                : res_->weightset()->conv(weight));
      res_->add_final(state_(s), w);
    }

    /// Add transitions from \a src to \a dst, labeled by \a entry.
    virtual void
    add_entry(const string_t& src, const string_t& dst,
              const string_t& entry) override final
    {
      auto s = state_(src);
      auto d = state_(dst);
      if (s == res_->pre() && d == res_->post())
        throw std::runtime_error("edit_automaton: invalid transition "
                                 "from pre to post: "
                                 + src.get() + " -> " + dst.get()
                                 + " (" + entry.get() + ")");
      if (s == res_->pre() || d == res_->post())
        {
          if (entry.get().empty())
            res_->add_transition(s, d, res_->prepost_label());
          else
            {
              // Adding a pre/post transition: be sure that it's only
              // a weight.  Entries see the special label as an empty
              // one.
              auto e = entryset_.conv(entry, sep_);
              if (e.size() == 1
                  && (res_->labelset()->is_special(begin(e)->first)
                      || res_->labelset()->is_one(begin(e)->first)))
                {
                  auto w = begin(e)->second;
                  res_->add_transition(s, d, res_->prepost_label(), w);
                }
              else
                throw std::runtime_error
                  (std::string{"edit_automaton: invalid "}
                   + (s == res_->pre() ? "initial" : "final")
                   + " entry: " + entry.get());
            }
        }
      else
        {
          auto p = emap_.emplace(entry, entry_t{});
          if (p.second)
            p.first->second = entryset_.conv(entry, sep_);
          add_entry(s, d, p.first->second);
        }
    }

    /// Return the built automaton.
    virtual dyn::detail::abstract_automaton*
    result() override final
    {
      return res_;
    }

    /// Detach the built automaton.
    virtual void
    reset() override final
    {
      res_ = nullptr;
    }

  private:
    /// Convert a state name to a state handler.
    state_t
    state_(const string_t& k)
    {
      auto p = smap_.emplace(k, Aut::null_state());
      if (p.second)
        p.first->second = res_->new_state();
      return p.first->second;
    }

    /// Add transitions between \a src and \a dst, for entry \a es.
    void
    add_entry(state_t src, state_t dst, const entry_t& es)
    {
      for (auto e: es)
        res_->add_transition(src, dst, e.first, e.second);
    }

    /// The automaton under construction.
    automaton_t* res_;
    /// Entries handler.
    entryset<context_t> entryset_;
    /// Map state name to state handler.
    state_map smap_;
    /// Memoize conversion from entry as a string to entry_t.
    entry_map emap_;
  };

  /// Build an automaton with unknown context.
  ///
  /// Record the transitions, initial and final states as strings,
  /// and once the automata completed, decide what is its
  /// context, and build a genuine automaton.
  class lazy_automaton_editor: public automaton_editor
  {
  public:
    using super_type = automaton_editor;
    using string_t = super_type::string_t;

  public:
    virtual void
    add_state(const string_t& s) override final
    {
      states_.emplace_back(s);
    }

    virtual void
    add_pre(const string_t&) override final
    {
      std::abort();
    }

    virtual void
    add_post(const string_t&) override final
    {
      std::abort();
    }

    virtual void
    add_initial(const string_t& s, const string_t& w = {}) override final
    {
      initial_states_.emplace_back(s, w);
    }

    virtual void
    add_final(const string_t& s, const string_t& w = {}) override final
    {
      final_states_.emplace_back(s, w);
    }

    /// Add transitions from \a src to \a dst, labeled by \a entry.
    virtual void
    add_entry(const string_t& src, const string_t& dst,
              const string_t& entry) override final;

    /// Return the built automaton.
    virtual dyn::detail::abstract_automaton*
    result() override final;

    /// Get ready to build another automaton.
    virtual void
    reset() override final;

  private:
    bool is_lan_ = false;
    bool is_law_ = false;
    std::set<char> letters_;
    std::vector<std::tuple<string_t, string_t, string_t>> transitions_;
    std::vector<std::pair<string_t, string_t>> initial_states_;
    std::vector<std::pair<string_t, string_t>> final_states_;
    std::vector<string_t> states_;
  };

  namespace dyn
  {
    namespace detail
    {
      template <typename Aut>
      automaton_editor*
      make_automaton_editor(const context& ctx)
      {
        const auto& c = ctx->as<const typename Aut::context_t&>();
        return new edit_automaton<Aut>(c);
      }

      REGISTER_DECLARE(make_automaton_editor,
                       (const context& aut) -> automaton_editor*);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
