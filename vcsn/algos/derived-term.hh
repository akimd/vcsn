#pragma once

#include <vcsn/algos/constant-term.hh>
#include <vcsn/algos/derivation.hh>
#include <vcsn/algos/split.hh>
#include <vcsn/algos/to-expansion.hh>
#include <vcsn/core/expression-automaton.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/polynomial.hh>
#include <vcsn/misc/getargs.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/weightset/polynomialset.hh>

namespace vcsn
{
  namespace detail
  {
    /// Specify a variety of derived-term construction.
    struct derived_term_algo
    {
      /// Core algorithms.
      enum algo_t
        {
          derivation,
          expansion,
        };

      derived_term_algo(algo_t a, bool b, bool d)
        : algo(a)
        , breaking(b)
        , determinize(d)
      {}

      /// From algo name to algo.
      derived_term_algo(const std::string& algo)
      {
        static const auto map = std::map<std::string, derived_term_algo>
          {
            //                          { algo, breaking, deterministic }.
            {"auto",                    {expansion,  false, false}},
            {"breaking_derivation",     {derivation, true,  false}},
            {"breaking_expansion",      {expansion,  true,  false}},
            {"derivation",              {derivation, false, false}},
            {"derivation,breaking",     {derivation, true,  false}},
            {"derivation,deterministic",{derivation, false,  true}},
            {"derivation_breaking",     {derivation, true,  false}},
            {"expansion",               {expansion,  false, false}},
            {"expansion,breaking",      {expansion,  true,  false}},
            {"expansion,deterministic", {expansion,  false, true}},
            {"expansion_breaking",      {expansion,  true,  false}},
          };
        *this = getargs("derived-term algorithm", map, algo);
      }

      /// Core algorithm.
      algo_t algo;
      /// Whether to break sums.
      bool breaking = false;
      /// Whether to determinize the expansions and produce a
      /// deterministic automaton, at the expense of possibly not
      /// terminating.
      bool determinize = false;
    };

    /*----------------------------.
    | derived_term(expression).   |
    `----------------------------*/

    /// Compute the derived-term automaton from an expression.
    ///
    /// Supports derivation/expansion as its core computation, with
    /// breaking/non breaking flavors for both.
    ///
    /// The handling of initial and final states can be simplified by
    /// working on a delimited rational expression (`$r$` with `$`
    /// being the special label), and mapping `$r$` to `pre()`, and
    /// `\e` to `post()`.  Then, there is no special treatment needed
    /// to handle the initial split in the case of a breaking
    /// derivation, and there is no special need to handle the final
    /// transitions.
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
    /// which is why we'd rather not have it at all.
    ///
    /// So, after experimentation, as of 2014-10, I prefer not to use
    /// the pre/post based construct in either case.
    template <typename ExpSet>
    struct derived_termer
    {
      using expressionset_t = ExpSet;
      using expression_t = typename expressionset_t::value_t;

      using context_t = context_t_of<expressionset_t>;
      using weightset_t = weightset_t_of<context_t>;

      using automaton_t = expression_automaton<mutable_automaton<context_t>>;
      using state_t = state_t_of<automaton_t>;

      derived_termer(const expressionset_t& rs, derived_term_algo algo)
        : rs_(rs)
        , algo_(algo)
        , res_{make_shared_ptr<automaton_t>(rs_)}
      {}

      /// Compute the derived-term automaton.
      automaton_t operator()(const expression_t& expression)
      {
        if (algo_.algo == derived_term_algo::derivation)
          return via_derivation(expression);
        else
          return via_expansion(expression);
      }

      /// Compute the derived-term automaton via derivation.
      automaton_t via_derivation(const expression_t& expression)
      {
        init_(expression);

        // The alphabet.
        const auto& ls = rs_.labelset()->generators();
        while (!res_->todo_.empty())
          {
            expression_t src = res_->todo_.top();
            state_t s = res_->state(src);
            res_->todo_.pop();
            res_->set_final(s, constant_term(rs_, src));
            for (auto l : ls)
              {
                auto p = derivation(rs_, src, l, algo_.breaking);
                if (algo_.determinize)
                  {
                    auto m = ps_.determinize(p);
                    res_->new_transition(s, label_of(m), l, weight_of(m));
                  }
                else
                  for (const auto& m: p)
                    res_->new_transition(s, label_of(m), l, weight_of(m));
              }
          }
        return res_;
      }

      /// Compute the derived-term automaton via expansion.
      automaton_t via_expansion(const expression_t& expression)
      {
        init_(expression);
        // Might be needed to determinize.
        auto es = rat::expansionset<expressionset_t>{rs_};

        auto to_expansion = rat::to_expansion_visitor<expressionset_t>{rs_};
        while (!res_->todo_.empty())
          {
            expression_t src = res_->todo_.top();
            res_->todo_.pop();
            auto s = res_->state(src);
            auto expansion = to_expansion(src);
            if (algo_.determinize)
              expansion = es.determinize(expansion);

            res_->set_final(s, expansion.constant);
            for (const auto& p: expansion.polynomials)
              if (algo_.breaking)
                for (const auto& m1: p.second)
                  for (const auto& m2: split(rs_, label_of(m1)))
                    res_->new_transition(s, label_of(m2), p.first,
                                         ws_.mul(weight_of(m1), weight_of(m2)));
              else if (algo_.determinize)
                {
                  auto m = ps_.determinize(p.second);
                  res_->new_transition(s, label_of(m), p.first, weight_of(m));
                }
              else
                for (const auto& m: p.second)
                  res_->new_transition(s, label_of(m), p.first, weight_of(m));
          }
        return res_;
      }

    private:
      void init_(const expression_t& expression)
      {
        if (algo_.breaking)
          for (const auto& p: split(rs_, expression))
            res_->set_initial(label_of(p), weight_of(p));
        else
          res_->set_initial(expression, ws_.one());
      }

      /// The expression's set.
      expressionset_t rs_;
      /// Its weightset.
      weightset_t ws_ = *rs_.weightset();
      /// The polynomialset for expressions.
      using polynomialset_t = rat::expression_polynomialset_t<expressionset_t>;
      polynomialset_t ps_ = make_expression_polynomialset(rs_);
      /// How derived terms are computed.
      derived_term_algo algo_;
      /// The resulting automaton.
      automaton_t res_;
    };
  }

  /// The derived-term automaton, for free labelsets.
  ///
  /// \param rs    the expressionset
  /// \param r     the expression
  /// \param algo  the algo to run: "auto", "derivation", or "expansion".
  template <typename ExpSet>
  inline
  std::enable_if_t<labelset_t_of<ExpSet>::is_free(),
    expression_automaton<mutable_automaton<typename ExpSet::context_t>>>
  derived_term(const ExpSet& rs,
               const typename ExpSet::value_t& r,
               const std::string& algo = "auto")
  {
    auto a = detail::derived_term_algo(algo);
    auto dt = detail::derived_termer<ExpSet>{rs, a};
    return dt(r);
  }

  /// The derived-term automaton, for non free labelsets.
  ///
  /// \param rs    the expressionset
  /// \param r     the expression
  /// \param algo  the algo to run: "auto", "derivation", or "expansion".
  template <typename ExpSet>
  inline
  std::enable_if_t<!labelset_t_of<ExpSet>::is_free(),
    expression_automaton<mutable_automaton<typename ExpSet::context_t>>>
  derived_term(const ExpSet& rs,
               const typename ExpSet::value_t& r,
               const std::string& algo = "auto")
  {
    auto a = detail::derived_term_algo(algo);
    require(a.algo == detail::derived_term_algo::expansion,
            "derived_term: cannot use derivation on non-free labelsets");
    // Do not call the operator(), this would trigger the compilation
    // of via_derivation, which does not compile (on purpose) for non
    // free labelsets.
    auto dt = detail::derived_termer<ExpSet>{rs, a};
    return dt.via_expansion(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet, typename String>
      inline
      automaton derived_term(const expression& exp, const std::string& algo)
      {
        const auto& e = exp->as<ExpSet>();
        const auto& rs = e.expressionset();
        const auto& r = e.expression();
        return make_automaton(::vcsn::derived_term(rs, r, algo));
      }
    }
  }
} // vcsn::
