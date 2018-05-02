#pragma once

#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <vcsn/core/name-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/misc/symbol.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{

  /*----------------------.
  | edit_automaton<Aut>.  |
  `----------------------*/

  /// Abstract Builder (the design pattern) for automata.
  class automaton_editor
  {
  public:
    using string_t = symbol;

    virtual ~automaton_editor() {}
    virtual void add_initial(string_t s, string_t w) = 0;
    virtual void add_final(string_t s, string_t w) = 0;

    /// Register the existence of state named \a s.
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
                                string_t weight = string_t{}) = 0;

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    virtual bool open(bool o) = 0;

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
  template <Automaton Aut>
  class edit_automaton: public automaton_editor
  {
  public:
    using super_t = automaton_editor;
    using automaton_t = name_automaton<Aut>;
    using string_t = super_t::string_t;

  private:
    using context_t = context_t_of<automaton_t>;
    using entry_t = typename polynomialset<context_t>::value_t;
    using state_t = state_t_of<automaton_t>;
    using label_t = label_t_of<automaton_t>;
    using labelset_t = labelset_t_of<automaton_t>;
    using weight_t = weight_t_of<automaton_t>;

  public:
    edit_automaton(const context_t& ctx)
      : res_(make_shared_ptr<automaton_t>(ctx))
      , ps_(ctx)
    {}

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    bool
    open(bool o) override final
    {
      return const_cast<labelset_t&>(*res_->context().labelset()).open(o);
    }

    /// Register the existence of state named \a s.
    void
    add_state(string_t s) override final
    {
      state_(s);
    }

    /// Register that state named \a s is preinitial.
    void
    add_pre(string_t s) override final
    {
      res_->state(s, res_->pre());
    }

    /// Register that state named \a s is postfinal.
    void
    add_post(string_t s) override final
    {
      res_->state(s, res_->post());
    }

    void
    add_initial(string_t s, string_t weight = string_t{}) override final
    {
      res_->add_initial(state_(s), weight_(weight));
    }

    void
    add_final(string_t s, string_t weight = string_t{}) override final
    {
      res_->add_final(state_(s), weight_(weight));
    }

    void
    add_transition(string_t src, string_t dst,
                   string_t label,
                   string_t weight = string_t{}) override final
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
    void
    add_entry(string_t src, string_t dst, string_t entry) override final
    {
      const auto s = state_(src);
      const auto d = state_(dst);
      require(s != res_->pre() || d != res_->post(),
              "edit_automaton: invalid transition from pre to post: ",
              src, " -> ", dst, " (", entry, ")");
      const auto& ls = *res_->labelset();
      if (s == res_->pre() || d == res_->post())
        {
          if (entry.get().empty())
            res_->add_transition(s, d, res_->prepost_label());
          else
            {
              using std::begin;
              // Adding a pre/post transition: be sure that it's only
              // a weight.
              const auto p = conv(ps_, entry, sep_);
              const auto m = *begin(p);
              VCSN_REQUIRE(p.size() == 1
                           && ls.is_special(label_of(m)),
                           "edit_automaton: invalid ",
                           s == res_->pre() ? "initial" : "final",
                           " entry: ", entry.get());
              res_->add_transition(s, d,
                                   res_->prepost_label(), weight_of(m));
            }
        }
      else
        {
          auto p = emap_.emplace(entry, entry_t{});
          if (p.second)
            p.first->second = conv(ps_, entry, sep_);
          for (auto e: p.first->second)
            res_->add_transition(s, d, label_of(e), weight_of(e));
        }
    }

    /// Return the built automaton.
    dyn::automaton
    result() override final
    {
      const_cast<labelset_t&>(*res_->context().labelset()).open(false);
      return res_;
    }

    /// Detach the built automaton.
    void
    reset() override final
    {
      res_ = nullptr;
    }

  private:
    /// Convert a label string to its value.
    label_t
    label_(string_t l)
    {
      const auto& ls = *res_->labelset();
      auto p = lmap_.emplace(l, label_t{});
      if (p.second)
        p.first->second = conv(ls, l);
      return p.first->second;
    }

    /// Convert a weight string to its value.
    weight_t
    weight_(string_t w)
    {
      const auto& ws = *res_->weightset();
      auto p = wmap_.emplace(w, weight_t{});
      if (p.second)
        p.first->second = w.get().empty() ? ws.one() : conv(ws, w);
      return p.first->second;
    }

    /// Convert a state name to a state handler.
    state_t
    state_(string_t k)
    {
      return res_->state(k);
    }

    /// The automaton under construction.
    automaton_t res_;
    /// Entries handler.
    polynomialset<context_t> ps_;

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
    /// A hash-cons'ed string type.
    using string_t = automaton_editor::string_t;

    /// Labelset types, increasing generality.
    enum class labelset_type { empty, lal, lan, law };

    /// Weightset types.
    enum class weightset_type { logarithmic, numerical, tropical };

    /// Add \a s as an initial state.
    void add_initial(string_t s, string_t w = string_t{});

    /// Add \a s as a final state.
    void add_final(string_t s, string_t w = string_t{});

    /// Add an acceptor transition from \a src to \a dst, labeled by
    /// \a lbl.
    void add_transition(string_t src, string_t dst,
                        string_t lbl, string_t w = string_t{});

    /// Add a transducer transition from \a src to \a dst, labeled by
    /// \a (lbl1, lbl2).
    void add_transition(string_t src, string_t dst,
                        string_t lbl1, string_t lbl2,
                        string_t w);

    /// Return the context that was inferred.
    std::string result_context() const;

    /// Return the built automaton.
    /// \param ctx  the context to load this automaton.
    ///    Defaults to the `result_context()`.
    dyn::automaton result(const std::string& ctx = {});

    /// Whether unknown letters should be added, or rejected.
    /// \param o   whether to accept
    /// \returns   the previous status.
    bool open(bool o);

    /// Get ready to build another automaton.
    void reset();

    /// Specify the weightset type.
    void weightset(weightset_type t) { weightset_type_ = t; };

  private:
    /// Record that this weight was seen.  Examine it to see if it's a
    /// float etc.
    void register_weight_(string_t w);

    /// The collected transitions: (Source, Destination, Label, Weight).
    using transition_t = std::tuple<string_t, string_t, string_t, string_t>;
    std::vector<transition_t> transitions_;
    /// The collected initial states: (State, Weight).
    std::vector<std::pair<string_t, string_t>> initial_states_;
    /// The collected final states: (State, Weight).
    std::vector<std::pair<string_t, string_t>> final_states_;
    /// Labelset type for input tape.
    labelset_type input_type_ = labelset_type::empty;
    /// Labelset type for output tape.
    labelset_type output_type_ = labelset_type::empty;
    /// Whether we saw a non-empty weight.
    bool weighted_ = false;
    /// Whether we saw a period in a the weight.
    bool real_ = false;
    /// Whether the labelset is open.
    bool open_ = false;
    /// The weightset scale.
    weightset_type weightset_type_ = weightset_type::numerical;
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
    }
  }
} // vcsn::
