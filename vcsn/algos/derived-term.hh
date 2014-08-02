#ifndef VCSN_ALGOS_DERIVED_TERM_HH
# define VCSN_ALGOS_DERIVED_TERM_HH

# include <vcsn/algos/constant-term.hh>
# include <vcsn/algos/derivation.hh>
# include <vcsn/algos/split.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/core/ratexp-automaton.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/dyn/polynomial.hh>
# include <vcsn/dyn/ratexp.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  /*-----------------------.
  | derived_term(ratexp).  |
  `-----------------------*/

  namespace detail
  {
    template <typename RatExpSet>
    struct derived_termer
    {
      using ratexpset_t = RatExpSet;
      using ratexp_t = typename ratexpset_t::value_t;

      using context_t = context_t_of<ratexpset_t>;
      using weightset_t = weightset_t_of<context_t>;

      using automaton_t = ratexp_automaton<mutable_automaton<context_t>>;
      using state_t = state_t_of<automaton_t>;

      /// Symbolic states: the derived terms are polynomials of ratexps.
      using polynomialset_t = rat::ratexp_polynomialset_t<ratexpset_t>;
      using polynomial_t = typename polynomialset_t::value_t;

      derived_termer(const ratexpset_t& rs, bool breaking = false)
        : rs_(rs)
        , breaking_(breaking)
        , res_{make_shared_ptr<automaton_t>(rs_.context())}
      {}

      automaton_t operator()(const ratexp_t& ratexp)
      {
        weightset_t ws = *rs_.weightset();
        // Turn the ratexp into a polynomial.
        {
          polynomial_t initial
            = breaking_ ? split(rs_, ratexp)
            : polynomial_t{{ratexp, ws.one()}};
          for (const auto& p: initial)
            // Also loads todo_.
            res_->set_initial(p.first, p.second);
        }

        const auto& ls = rs_.labelset()->genset();
        while (!res_->todo_.empty())
          {
            ratexp_t src = res_->todo_.top();
            auto s = res_->state(src);
            res_->todo_.pop();
            res_->set_final(s, constant_term(rs_, src));
            for (auto l : ls)
              for (const auto& m: derivation(rs_, src, l, breaking_))
                res_->add_transition(s, m.first, l, m.second);
          }
        return std::move(res_);
      }

    private:
      /// The ratexp's set.
      ratexpset_t rs_;
      /// Whether to perform a breaking derivation.
      bool breaking_ = false;
      /// The resulting automaton.
      automaton_t res_;
    };
  }

  /// Derive a ratexp wrt to a string.
  template <typename RatExpSet>
  inline
  ratexp_automaton<mutable_automaton<typename RatExpSet::context_t>>
  derived_term(const RatExpSet& rs, const typename RatExpSet::ratexp_t& r,
               bool breaking = false)
  {
    detail::derived_termer<RatExpSet> dt{rs, breaking};
    return dt(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename RatExpSet, typename Book>
      automaton
      derived_term(const ratexp& exp, bool breaking = false)
      {
        const auto& r = exp->as<RatExpSet>();
        return make_automaton(::vcsn::derived_term(r.ratexpset(),
                                                   r.ratexp(), breaking));
      }

      REGISTER_DECLARE(derived_term,
                       (const ratexp& e, bool breaking) -> automaton);
    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_DERIVED_TERM_HH
