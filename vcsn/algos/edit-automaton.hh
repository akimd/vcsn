#ifndef VCSN_ALGOS_EDIT_AUTOMATON_HH
# define VCSN_ALGOS_EDIT_AUTOMATON_HH

# include <unordered_map>

# include <boost/flyweight.hpp>
# include <boost/flyweight/no_tracking.hpp>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/ctx/fwd.hh>

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
    virtual void add_state(const string_t& s) = 0;
    virtual void add_pre(const string_t& s) = 0;
    virtual void add_post(const string_t& s) = 0;
    virtual void add_entry(const string_t& src, const string_t& dst,
                           const string_t& entry) = 0;
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
    using super_type = automaton_editor;
    using automaton_t = Aut;
    using string_t = super_type::string_t;

  private:
    using context_t = typename automaton_t::context_t;
    using entry_t = typename automaton_t::entry_t;
    using state_t = typename automaton_t::state_t;

    using state_map = std::unordered_map<string_t, state_t>;
    using entry_map = std::unordered_map<string_t, entry_t>;

  public:
    edit_automaton(const context_t& ctx)
      : res_(new automaton_t(ctx))
      , unit_(res_->entryset().unit())
    {}

    ~edit_automaton()
    {
      delete res_;
    }

    virtual void
    add_state(const string_t& s) override final
    {
      state_(s);
    }

    virtual void
    add_pre(const string_t& s) override final
    {
      smap_.emplace(s, res_->pre());
    }

    virtual void
    add_post(const string_t& s) override final
    {
      smap_.emplace(s, res_->post());
    }

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
              auto e = res_->entryset().conv(entry, sep_);
              if (e.size() != 1
                  || !res_->labelset()->is_empty_word(begin(e)->first))
                throw std::runtime_error
                  (std::string{"edit_automaton: invalid "}
                   + (s == res_->pre() ? "initial" : "final")
                   + " entry: " + entry.get());
              auto w = begin(e)->second;
              res_->add_transition(s, d, res_->prepost_label(), w);
            }
        }
      else
        {
          auto p = emap_.emplace(entry, unit_);
          if (p.second)
            p.first->second = res_->entryset().conv(entry, sep_);
          res_->add_entry(s, d, p.first->second);
        }
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
    state_(const string_t& k)
    {
      auto p = smap_.emplace(k, Aut::null_state());
      if (p.second)
        p.first->second = res_->new_state();
      return p.first->second;
    }

    automaton_t* res_;
    entry_t unit_;
    state_map smap_;
    entry_map emap_;
  };

  namespace dyn
  {
    namespace details
    {
      template <typename Aut>
      automaton_editor*
      make_automaton_editor(const dyn::context& ctx)
      {
        const auto& c = dynamic_cast<const typename Aut::context_t&>(*ctx);
        return new edit_automaton<Aut>(c);
      }

      REGISTER_DECLARE(make_automaton_editor,
                       (const dyn::context& aut) -> automaton_editor*);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
