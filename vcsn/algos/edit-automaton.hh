#ifndef VCSN_ALGOS_EDIT_AUTOMATON_HH
# define VCSN_ALGOS_EDIT_AUTOMATON_HH

# include <set>
# include <tuple>
# include <unordered_map>
# include <utility>
# include <vector>

# include <vcsn/core/mutable_automaton.hh>
# include <vcsn/ctx/context.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/algos.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/context.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/misc/flyweight.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/stream.hh>
# include <vcsn/weightset/polynomialset.hh>

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
    virtual void add_initial(string_t s, string_t w) = 0;
    virtual void add_final(string_t s, string_t w) = 0;

    virtual void add_state(string_t s) = 0;

    /// Declare that \a s denotes the preinitial state in entries.
    virtual void add_pre(string_t s) = 0;

    /// Declare that \a s denotes the postfinal state in entries.
    virtual void add_post(string_t s) = 0;

    /// Add an entry from \a src to \a dst, with value \a entry.
    virtual void add_entry(string_t src, string_t dst,
                           string_t entry) = 0;

    /// Add a transition from \a src to \a dst.
    virtual void add_transition(string_t src, string_t dst,
                                string_t label,
                                string_t weight = {}) = 0;

    /// The final result.
    virtual dyn::automaton result() = 0;
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
    using entry_t = typename polynomialset<context_t>::value_t;
    using state_t = typename automaton_t::state_t;
    using label_t = typename automaton_t::label_t;
    using weight_t = typename automaton_t::weight_t;

  public:
    edit_automaton(const context_t& ctx)
      : res_(new automaton_t(ctx))
      , ps_(ctx)
    {}

    ~edit_automaton()
    {
      delete res_;
    }

    /// Register the existence of state named \a s.
    virtual void
    add_state(string_t s) override final
    {
      state_(s);
    }

    /// Register that state named \a s is preinitial.
    virtual void
    add_pre(string_t s) override final
    {
      smap_.emplace(s, res_->pre());
    }

    /// Register that state named \a s is postfinal.
    virtual void
    add_post(string_t s) override final
    {
      smap_.emplace(s, res_->post());
    }

    virtual void
    add_initial(string_t s, string_t weight = {}) override final
    {
      res_->add_initial(state_(s), weight_(weight));
    }

    virtual void
    add_final(string_t s, string_t weight = {}) override final
    {
      res_->add_final(state_(s), weight_(weight));
    }

    virtual void
    add_transition(string_t src, string_t dst,
                   string_t label,
                   string_t weight = {}) override final
    {
      // In case of states we don't know, we'd like to register s
      // first, then d, in an attempt to keep the order in which we
      // discover states.  Which is not guaranteed by plain argument
      // evaluation.
      auto s = state_(src);
      auto d = state_(dst);
      res_->add_transition(s, d, label_(label), weight_(weight));
    }

    /// Add transitions from \a src to \a dst, labeled by \a entry.
    virtual void
    add_entry(string_t src, string_t dst, string_t entry) override final
    {
      auto s = state_(src);
      auto d = state_(dst);
      if (s == res_->pre() && d == res_->post())
        raise("edit_automaton: invalid transition from pre to post: "
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
              auto e = conv(ps_, entry, sep_);
              if (e.size() == 1
                  && (res_->labelset()->is_special(begin(e)->first)
                      || res_->labelset()->is_one(begin(e)->first)))
                {
                  auto w = begin(e)->second;
                  res_->add_transition(s, d, res_->prepost_label(), w);
                }
              else
                raise(std::string{"edit_automaton: invalid "}
                      + (s == res_->pre() ? "initial" : "final")
                      + " entry: " + entry.get());
            }
        }
      else
        {
          auto p = emap_.emplace(entry, entry_t{});
          if (p.second)
            p.first->second = conv(ps_, entry, sep_);
          for (auto e: p.first->second)
            res_->add_transition(s, d, e.first, e.second);
        }
    }

    /// Return the built automaton.
    virtual dyn::automaton
    result() override final
    {
      return dyn::make_automaton(std::move(*res_));
    }

    /// Detach the built automaton.
    virtual void
    reset() override final
    {
      res_ = nullptr;
    }

  private:
    /// Convert a label string to its value.
    label_t
    label_(string_t l)
    {
      static const auto& ls = *res_->labelset();
      auto p = lmap_.emplace(l, label_t{});
      if (p.second)
        p.first->second = conv(ls, l);
      return p.first->second;
    }

    /// Convert a weight string to its value.
    weight_t
    weight_(string_t w)
    {
      static const auto& ws = *res_->weightset();
      auto p = wmap_.emplace(w, weight_t{});
      if (p.second)
        p.first->second = w.get().empty() ? ws.one() : conv(ws, w);
      return p.first->second;
    }

    /// Convert a state name to a state handler.
    state_t
    state_(string_t k)
    {
      auto p = smap_.emplace(k, Aut::null_state());
      if (p.second)
        p.first->second = res_->new_state();
      return p.first->second;
    }

    /// The automaton under construction.
    automaton_t* res_;
    /// Entries handler.
    polynomialset<context_t> ps_;

    /// State name -> state handle.
    using state_map = std::unordered_map<string_t, state_t>;
    state_map smap_;
    /// Memoize entry conversion.
    using entry_map = std::unordered_map<string_t, entry_t>;
    entry_map emap_;
    /// Memoize label conversion.
    using label_map = std::unordered_map<string_t, label_t>;
    label_map lmap_;
    /// Memoize weight conversion.
    using weight_map = std::unordered_map<string_t, weight_t>;
    weight_map wmap_;
  };

  /// Build an automaton with unknown context.
  ///
  /// Record the transitions, initial and final states as strings,
  /// and once the automata completed, decide what is its
  /// context, and build a genuine automaton.
  class lazy_automaton_editor
  {
  public:
    using string_t = automaton_editor::string_t;

  public:
    /// Add \a s as an initial state.
    void add_initial(string_t s, string_t w = {});

    /// Add \a s as a final state.
    void add_final(string_t s, string_t w = {});

    /// Add a transition from \a src to \a dst, labeled by \a lbl.
    void add_transition(string_t src, string_t dst,
                        string_t lbl, string_t w = {});

    /// Return the built automaton.
    dyn::automaton result();

    /// Get ready to build another automaton.
    void reset();

  private:
    /// Whether we saw a "\e" as label.
    bool is_lan_ = false;
    /// Whether we saw a multi-chars label.
    bool is_law_ = false;
    /// The collected letters from the labels.
    std::set<char> letters_;
    /// Whether we saw a non-empty weight.
    bool weighted_ = false;
    /// Whether we saw a period in a the weight.
    bool real_ = false;
    /// The collected transitions: (Source, Destination, Label, Weight).
    std::vector<std::tuple<string_t, string_t, string_t, string_t>> transitions_;
    /// The collected initial states: (State, Weight).
    std::vector<std::pair<string_t, string_t>> initial_states_;
    /// The collected final states: (State, Weight).
    std::vector<std::pair<string_t, string_t>> final_states_;
  };

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Ctx>
      automaton_editor*
      make_automaton_editor(const context& ctx)
      {
        const auto& c = ctx->as<Ctx>();
        return new edit_automaton<mutable_automaton<Ctx>>(c);
      }

      REGISTER_DECLARE(make_automaton_editor,
                       (const context& ctx) -> automaton_editor*);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
