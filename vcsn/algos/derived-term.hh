#ifndef VCSN_ALGOS_DERIVED_TERM_HH
# define VCSN_ALGOS_DERIVED_TERM_HH

# include <vcsn/algos/constant-term.hh>
# include <vcsn/algos/derivation.hh>
# include <vcsn/algos/to-expansion.hh>
# include <vcsn/algos/split.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/core/expression-automaton.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/expression.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  /*-----------------------.
  | derived_term(expression).  |
  `-----------------------*/

  namespace detail
  {
    /// Compute the derived-term automaton from an expression.
    ///
    /// Supports derivation/expansion as its core computation,
    /// with breaking/non breaking flavors for both.
    ///
    /// The handling of initial and final states can be simplified by
    /// working on a delimited rational expression ('$r$' with '$'
    /// being the special label), and mapping '$r$' to pre(), and '\e'
    /// to post().  Then, there is no special treatment needed to
    /// handle the initial split in the case of a breaking derivation,
    /// and there is no special need to handle the final transitions.
    ///
    /// However there are two problems.
    ///
    /// One is that derivation loops over the alphabets, so we have to
    /// add $, the special label, to the alphabet.  But then, the
    /// handling of complement starts making idiotic things by trying
    /// to add $-labels in inner transitions.  This is a real problem,
    /// with no clear work-around.
    ///
    /// But this is not an issue for expansion-based construction.
    /// However it means that the states are no longer labeled by the
    /// expressions as the user would expect them: there is always the
    /// terminating $.  Not showing it does not address the issue,
    /// which is that we'd rather not have it at all.
    ///
    /// So, after experimentation, as of 2014-10, I prefer not to use
    /// the pre/post based construct in neither case.
    template <typename RatExpSet>
    struct derived_termer
    {
      using expressionset_t = RatExpSet;
      using expression_t = typename expressionset_t::value_t;

      using context_t = context_t_of<expressionset_t>;
      using weightset_t = weightset_t_of<context_t>;

      using automaton_t = expression_automaton<mutable_automaton<context_t>>;
      using state_t = state_t_of<automaton_t>;

      /// Symbolic states: the derived terms are polynomials of expressions.
      using polynomialset_t = rat::expression_polynomialset_t<expressionset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      derived_termer(const expressionset_t& rs, bool breaking = false)
        : rs_(rs)
        , breaking_(breaking)
        , res_{make_shared_ptr<automaton_t>(rs_.context())}
      {}

      /// Compute the derived-term automaton via derivation.
      automaton_t via_derivation(const expression_t& expression)
      {
        init_(expression);

        // The alphabet.
        const auto& ls = rs_.labelset()->genset();
        while (!res_->todo_.empty())
          {
            expression_t src = res_->todo_.top();
            state_t s = res_->state(src);
            res_->todo_.pop();
            res_->set_final(s, constant_term(rs_, src));
            for (auto l : ls)
              for (const auto& m: derivation(rs_, src, l, breaking_))
                res_->new_transition(s, m.first, l, m.second);
          }
        return res_;
      }

      /// Compute the derived-term automaton via expansion.
      automaton_t via_expansion(const expression_t& expression)
      {
        init_(expression);

        rat::to_expansion_visitor<expressionset_t> expand{rs_};
        while (!res_->todo_.empty())
          {
            expression_t src = res_->todo_.top();
            auto s = res_->state(src);
            res_->todo_.pop();
            auto expansion = expand(src);
            res_->set_final(s, expansion.constant);
            for (const auto& p: expansion.polynomials)
              if (breaking_)
                for (const auto& m1: p.second)
                  for (const auto& m2: split(rs_, m1.first))
                    res_->new_transition(s, m2.first, p.first,
                                         ws_.mul(m1.second, m2.second));
              else
                for (const auto& m: p.second)
                  res_->new_transition(s, m.first, p.first, m.second);
          }
        return res_;
      }

    private:
      void init_(const expression_t& expression)
      {
        if (breaking_)
          for (const auto& p: split(rs_, expression))
            res_->set_initial(p.first, p.second);
        else
          res_->set_initial(expression, ws_.one());
      }

      /// The expression's set.
      expressionset_t rs_;
      /// Its weightset.
      weightset_t ws_ = *rs_.weightset();
      /// Whether to break the polynomials.
      bool breaking_ = false;
      /// The resulting automaton.
      automaton_t res_;
    };
  }

  /// The derived-term automaton, computed by derivation.
  template <typename RatExpSet>
  inline
  expression_automaton<mutable_automaton<typename RatExpSet::context_t>>
  derived_term_derivation(const RatExpSet& rs,
                          const typename RatExpSet::value_t& r,
                          bool breaking = false)
  {
    detail::derived_termer<RatExpSet> dt{rs, breaking};
    return dt.via_derivation(r);
  }

  /// The derived-term automaton, computed by expansion.
  template <typename RatExpSet>
  inline
  expression_automaton<mutable_automaton<typename RatExpSet::context_t>>
  derived_term_expansion(const RatExpSet& rs,
                         const typename RatExpSet::value_t& r,
                         bool breaking = false)
  {
    detail::derived_termer<RatExpSet> dt{rs, breaking};
    return dt.via_expansion(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename String>
      inline
      typename std::enable_if<RatExpSet::context_t::labelset_t::is_free(),
                              automaton>::type
      derived_term(const expression& exp, const std::string& algo)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.expressionset();
        const auto& r = e.expression();
        auto res
          = algo == "derivation"
          ? ::vcsn::derived_term_derivation(rs, r)
          : algo == "breaking_derivation"
          ? ::vcsn::derived_term_derivation(rs, r, true)
          : algo == "auto" || algo == "expansion"
          ? ::vcsn::derived_term_expansion(rs, r)
          : algo == "breaking_expansion"
          ? ::vcsn::derived_term_expansion(rs, r, true)
          : nullptr;
        require(!!res, "derived_term: invalid algorithm: " + algo);
        return make_automaton(res);
      }

      /// Bridge.
      template <typename RatExpSet, typename String>
      inline
      typename std::enable_if<!RatExpSet::context_t::labelset_t::is_free(),
                              automaton>::type
      derived_term(const expression& exp, const std::string& algo)
      {
        const auto& e = exp->as<RatExpSet>();
        const auto& rs = e.expressionset();
        const auto& r = e.expression();
        auto res
          = algo == "auto" || algo == "expansion"
          ? ::vcsn::derived_term_expansion(rs, r)
          : algo == "breaking_expansion"
          ? ::vcsn::derived_term_expansion(rs, r, true)
          : nullptr;
        require(!!res, "derived_term: invalid algorithm: " + algo);
        return make_automaton(res);
      }

      REGISTER_DECLARE(derived_term,
                       (const expression& e, const std::string& algo) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_DERIVED_TERM_HH
