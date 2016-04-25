#pragma once

#include <vcsn/algos/constant-term.hh>
#include <vcsn/algos/derivation.hh>
#include <vcsn/algos/split.hh>
#include <vcsn/algos/to-expansion.hh>
#include <vcsn/core/expression-automaton.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/value.hh>
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
      derived_term_algo(std::string algo)
      {
        using dta = derived_term_algo;
        static const auto map = getarg<derived_term_algo>
          {
            "derived-term algorithm",
            {
              //                          { algo, breaking, deterministic }.
              {"auto",                    dta{expansion,  false, false}},
              {"breaking_derivation",     "derivation,breaking"},
              {"breaking_expansion",      "expansion,breaking"},
              {"derivation",              dta{derivation, false, false}},
              {"derivation,breaking",     dta{derivation, true,  false}},
              {"derivation,deterministic",dta{derivation, false,  true}},
              {"derivation_breaking",     "derivation,breaking"},
              {"expansion",               dta{expansion,  false, false}},
              {"expansion,breaking",      dta{expansion,  true,  false}},
              {"expansion,deterministic", dta{expansion,  false, true}},
              {"expansion_breaking",      "expansion,breaking"},
            }
          };
        if (boost::starts_with(algo, "lazy,"))
          algo = algo.substr(5);
        *this = map[algo];
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

    /*--------------------------.
    | derived_term_automaton.   |
    `--------------------------*/

    /// Additional members when the labelset is free.
    template <typename ExpSet,
              bool = labelset_t_of<ExpSet>::is_free()>
    struct derived_term_automaton_members
    {
      derived_term_automaton_members(const ExpSet& rs)
        : gens{rs.labelset()->generators()}
      {}

      /// The alphabet.
      using genset_t = typename labelset_t_of<ExpSet>::genset_t;
      genset_t gens;
    };

    /// Additional members when the labelset is not free.
    template <typename ExpSet>
    struct derived_term_automaton_members<ExpSet, false>
    {
      derived_term_automaton_members(const ExpSet&){}
    };

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
    class derived_term_automaton_impl
      : public automaton_decorator<expression_automaton<mutable_automaton<context_t_of<ExpSet>>>>
    {
    public:
      using expressionset_t = ExpSet;
      /// Our state names: expressions.
      using expression_t = typename expressionset_t::value_t;

      using context_t = context_t_of<expressionset_t>;
      using weightset_t = weightset_t_of<context_t>;

      /// The type of the (strict) automaton we build.
      using automaton_t = expression_automaton<mutable_automaton<context_t>>;
      using state_t = state_t_of<automaton_t>;

      /// This class.
      using self_t = derived_term_automaton_impl;
      /// Base class.
      using super_t = automaton_decorator<automaton_t>;

      static symbol sname()
      {
        static auto res = symbol{"derived_term_automaton<"
                                 + expressionset_t::sname()
                                 + ">"};
        return res;
      }

      std::ostream& print_set(std::ostream& o, format fmt = {}) const
      {
        o << "derived_term_automaton<";
        return rs_.print_set(o, fmt) << '>';
      }

      derived_term_automaton_impl(const expressionset_t& rs,
                                  derived_term_algo algo)
        : super_t{make_shared_ptr<automaton_t>(rs)}
        , rs_{rs}
        , algo_{algo}
        , members_{rs}
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
        while (!aut_->todo_.empty())
          {
            auto p = std::move(aut_->todo_.top());
            aut_->todo_.pop();
            complete_via_derivation_(p.first, p.second);
          }
        return aut_;
      }

      /// Compute the derived-term automaton via expansion.
      automaton_t via_expansion(const expression_t& expression)
      {
        init_(expression);
        while (!aut_->todo_.empty())
          {
            auto p = std::move(aut_->todo_.top());
            aut_->todo_.pop();
            complete_via_expansion_(p.first, p.second);
          }
        return aut_;
      }

      //    private:
      /// The expression_automaton we are building.
      using super_t::aut_;

      /// Initialize the computation: build the initial states.
      void init_(const expression_t& expression)
      {
        if (algo_.breaking)
          for (const auto& p: split(rs_, expression))
            aut_->set_initial(label_of(p), weight_of(p));
        else
          aut_->set_initial(expression, ws_.one());
      }

      /// Complete a state: find its outgoing transitions.
      void complete_(state_t s) const
      {
        const auto& orig = aut_->origins();
        auto sn = orig.at(s);
        const_cast<self_t&>(*this).complete_via_expansion_(s, sn);
      }

      /// All the outgoing transitions.
      auto all_out(state_t s) const
        -> decltype(vcsn::detail::all_out(aut_, s))
      {
        if (this->is_lazy(s))
          complete_(s);
        return vcsn::detail::all_out(aut_, s);
      }

      /// Compute the outgoing transitions of \a src.
      template <typename ES = expressionset_t,
                typename = std::enable_if<labelset_t_of<ES>::is_free()>>
      void complete_via_derivation_(state_t s, const expression_t& src)
      {
        aut_->set_lazy(s, false);
        aut_->set_final(s, constant_term(rs_, src));
        for (auto l : members_.gens)
          {
            auto p = derivation(rs_, src, l, algo_.breaking);
            if (algo_.determinize)
              {
                auto m = ps_.determinize(p);
                aut_->new_transition(s, label_of(m), l, weight_of(m));
              }
            else
              for (const auto& m: p)
                aut_->new_transition(s, label_of(m), l, weight_of(m));
          }
      }

      /// Compute the outgoing transitions of \a src.
      void complete_via_expansion_(state_t s, const expression_t& src)
      {
        aut_->set_lazy(s, false);
        auto expansion = to_expansion_(src);
        if (algo_.determinize)
          expansion = es_.determinize(expansion);

        aut_->set_final(s, expansion.constant);
        for (const auto& p: expansion.polynomials)
          if (algo_.breaking)
            for (const auto& m1: p.second)
              for (const auto& m2: split(rs_, label_of(m1)))
                aut_->new_transition(s, label_of(m2), p.first,
                                     ws_.mul(weight_of(m1), weight_of(m2)));
          else if (algo_.determinize)
            {
              auto m = ps_.determinize(p.second);
              aut_->new_transition(s, label_of(m), p.first, weight_of(m));
            }
          else
            for (const auto& m: p.second)
              aut_->new_transition(s, label_of(m), p.first, weight_of(m));
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

      /// Might be needed to determinize.
      using expansionset_t = rat::expansionset<expressionset_t>;
      expansionset_t es_ = {rs_};
      /// Used for expansions.
      using to_expansion_t = rat::to_expansion_visitor<expressionset_t>;
      to_expansion_t to_expansion_ = {rs_};
      /// Possibly the generators.
      derived_term_automaton_members<expressionset_t> members_ = {rs_};
    };
  }

  /// A derived-term automaton as a shared pointer.
  template <typename ExpSet>
  using derived_term_automaton
    = std::shared_ptr<detail::derived_term_automaton_impl<ExpSet>>;

  template <typename ExpSet>
  auto
  make_derived_term_automaton(const ExpSet& rs,
                              const detail::derived_term_algo& algo)
    -> derived_term_automaton<ExpSet>
  {
    using res_t = derived_term_automaton<ExpSet>;
    return make_shared_ptr<res_t>(rs, algo);
  }

  /// The derived-term automaton, for free labelsets.
  ///
  /// \param rs    the expressionset
  /// \param r     the expression
  /// \param algo  the algo to run: "auto", "derivation", or "expansion".
  template <typename ExpSet>
  std::enable_if_t<labelset_t_of<ExpSet>::is_free(),
    expression_automaton<mutable_automaton<typename ExpSet::context_t>>>
  derived_term(const ExpSet& rs,
               const typename ExpSet::value_t& r,
               const std::string& algo = "auto")
  {
    auto a = detail::derived_term_algo(algo);
    auto dt = make_derived_term_automaton(rs, a);
    return dt->operator()(r);
  }

  /// The derived-term automaton, for non free labelsets.
  ///
  /// \param rs    the expressionset
  /// \param r     the expression
  /// \param algo  the algo to run: "auto", "derivation", or "expansion".
  template <typename ExpSet>
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
    auto dt = make_derived_term_automaton(rs, a);
    return dt->via_expansion(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename ExpSet, typename String>
      automaton derived_term(const expression& exp, const std::string& algo)
      {
        const auto& e = exp->as<ExpSet>();
        const auto& rs = e.valueset();
        const auto& r = e.value();
        if (boost::starts_with(algo, "lazy"))
          {
            auto a = vcsn::detail::derived_term_algo(algo);
            require(a.algo == vcsn::detail::derived_term_algo::expansion,
                    "derived_term: laziness works only with expansions");
            // Do not call the operator(), this would trigger the compilation
            // of via_derivation, which does not compile (on purpose) for non
            // free labelsets.
            auto res = make_derived_term_automaton(rs, a);
            res->init_(r);
            return res;
          }
        else
          return ::vcsn::derived_term(rs, r, algo);
      }
    }
  }
} // vcsn::
