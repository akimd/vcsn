#ifndef VCSN_ALGOS_EDIT_AUTOMATON_HH
# define VCSN_ALGOS_EDIT_AUTOMATON_HH

# include <map>
# include <vcsn/core/fwd.hh>
# include <vcsn/ctx/fwd.hh>

namespace vcsn
{

  /*-----------------.
  | add_entry<Aut>.  |
  `-----------------*/

  /// Add transitions from src to dst, labeled by \c entry.
  template <typename Aut>
  void
  add_entry(Aut& a,
            typename Aut::state_t src, typename Aut::state_t dst,
            const std::string& entry, const char sep = '+')
  {
    assert (src != a.pre() || dst != a.post());
    auto w = a.entryset().conv(entry, sep);
    // Initial and final weights are _not_ labeled by the empty word.
    // Yet, the dot parser does not know that.
    // FIXME: decide whose responsibility this is.
    if (src == a.pre() || dst == a.post())
      {
        assert(w.size() == 1);
        assert(a.labelset()->is_identity(begin(w)->first));
        a.add_transition(src, dst, a.prepost_label(), begin(w)->second);
      }
    else
      a.add_entry(src, dst, w);
  }

  /// Add transitions from src to dst, possibly being pre() xor post().
  template <typename Aut>
  void
  add_entry(Aut& a,
            typename Aut::state_t src, typename Aut::state_t dst,
            const std::string* entry, const char sep = '+')
  {
    if (entry)
      add_entry(a, src, dst, *entry, sep);
    else
      {
        assert (src == a.pre() || dst == a.post());
        a.add_transition(src, dst, a.prepost_label());
      }
  }

  /*----------------------.
  | edit_automaton<Aut>.  |
  `----------------------*/

  /// Abstract Builder (the design pattern) for automata.
  class automaton_editor
  {
  public:
    virtual ~automaton_editor() {}
    virtual void add_state(const std::string& s) = 0;
    virtual void add_pre(const std::string& s) = 0;
    virtual void add_post(const std::string& s) = 0;
    virtual void add_entry(const std::string& src, const std::string& dst,
                           const std::string* entry) = 0;
    /// The final result.
    virtual dyn::abstract_automaton* result() = 0;
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
    using automaton_t = Aut;
  private:
    using state_t = typename automaton_t::state_t;
    using context_t = typename automaton_t::context_t;
    using state_map = std::map<std::string, state_t>;

  public:
    edit_automaton(const context_t& ctx)
      : res_(new automaton_t(ctx))
    {}

    ~edit_automaton()
    {
      delete res_;
    }

    virtual void
    add_state(const std::string& s) override final
    {
      state_(s);
    }

    virtual void
    add_pre(const std::string& s) override final
    {
      smap_.insert({s, res_->pre()});
    }

    virtual void
    add_post(const std::string& s) override final
    {
      smap_.insert({s, res_->post()});
    }

    virtual void
    add_entry(const std::string& src, const std::string& dst,
              const std::string* entry) override final
    {
      vcsn::add_entry(*res_, state_(src), state_(dst), entry, sep_);
    }

    virtual dyn::abstract_automaton*
    result() override final
    {
      return res_;
    }

    virtual void
    reset() override final
    {
      res_ = nullptr;
    }



  private:
    state_t
    state_(const std::string& k)
    {
      // See ``Efficient STL''.
      auto i = smap_.lower_bound(k);
      state_t res;
      if (i == end(smap_) || smap_.key_comp()(k, i->first))
        {
          // First insertion.
          res = res_->new_state();
          smap_.insert({k, res});
        }
      else
        res = i->second;
      return res;
    }

    state_map smap_;
    automaton_t* res_;
  };

  template <typename Aut>
  automaton_editor*
  abstract_make_automaton_editor(const dyn::context& ctx)
  {
    const auto& c = dynamic_cast<const typename Aut::context_t&>(*ctx);
    return new edit_automaton<Aut>(c);
  }

  using make_automaton_editor_t =
    auto (const dyn::context& aut) -> automaton_editor*;

  bool
  make_automaton_editor_register(const std::string& ctx,
                                 const make_automaton_editor_t& fn);

  /// Abstract.
  automaton_editor*
  make_automaton_editor(const dyn::context& ctx);

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
