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
# include <vcsn/weights/polynomialset.hh>

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

    /// State name -> state handle.
    using state_map = std::unordered_map<string_t, state_t>;
    using entry_map = std::unordered_map<string_t, entry_t>;

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
    add_transition(string_t s, string_t d,
                   string_t label,
                   string_t weight = {}) override final
    {
      res_->add_transition(state_(s), state_(d),
                           label_(label), weight_(weight));
    }

    /// Add transitions from \a src to \a dst, labeled by \a entry.
    virtual void
    add_entry(string_t src, string_t dst,
              string_t entry) override final
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
              auto e = ps_.conv(entry, sep_);
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
            p.first->second = ps_.conv(entry, sep_);
          add_entry(s, d, p.first->second);
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
      return ls.conv(l);
    }

    /// Convert a weight string to its value.
    weight_t
    weight_(string_t w)
    {
      static const auto& ws = *res_->weightset();
      return w.get().empty() ? ws.one() : ws.conv(w);
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
    polynomialset<context_t> ps_;
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
  class lazy_automaton_editor
  {
  public:
    using string_t = automaton_editor::string_t;

  public:
    void add_state(string_t s);

    void add_initial(string_t s, string_t w = {});

    void add_final(string_t s, string_t w = {});

    /// Add transitions from \a src to \a dst, labeled by \a lbl.
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
    /// The collected state names.  Keep sorted as an attempt to
    /// keep their order ("0" -> 0, etc.).
    // FIXME: we'd need to have "11" > "2", which is not the case here.
    std::set<string_t> states_;
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
