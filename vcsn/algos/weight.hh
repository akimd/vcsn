#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/algos/tags.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/core/rat/expansionset.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/value.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{
  /*----------------.
  | Function tags.  |
  `----------------*/

  CREATE_FUNCTION_TAG(lweight);
  CREATE_FUNCTION_TAG(rweight);

  namespace detail
  {
    /// Implementation of left- and right- multiplication of an
    /// automaton by a weight.
    template <Automaton Aut>
    struct standard_operations
    {
      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      using weight_t = weight_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;
      using state_t = state_t_of<automaton_t>;

      /// Left-multiplication of any automaton by a weight.
      static automaton_t&
      lweight_here(const weight_t& w, automaton_t& res, general_tag tag)
      {
        const auto& ws = *res->context().weightset();
        if (ws.is_zero(w))
          zero_here(res, tag);
        else if (!ws.is_one(w))
          for (auto t: initial_transitions(res))
            res->lweight(t, w);
        res->properties().update(lweight_ftag{});
        return res;
      }

      /// Standard-preserving left-multiplication by a weight.
      static automaton_t&
      lweight_here(const weight_t& w, automaton_t& res, standard_tag tag)
      {
        require(is_standard(res), __func__, ": automaton must be standard");
        const auto& ws = *res->context().weightset();
        if (ws.is_zero(w))
          zero_here(res, tag);
        else if (!ws.is_one(w))
          {
            state_t initial = res->dst_of(initial_transitions(res).front());
            for (auto t: all_out(res, initial))
              res->lweight(t, w);
          }
        res->properties().update(lweight_ftag{});
        return res;
      }

      /// Same as standard if res is standard, otherwise, general.
      static automaton_t&
      lweight_here(const weight_t& w, automaton_t& res, auto_tag = {})
      {
        if (is_standard(res))
          return lweight_here(w, res, standard_tag{});
        else
          return lweight_here(w, res, general_tag{});
      }

      /// Right-multiplication of any automaton by a weight.
      template <typename Tag = general_tag>
      static automaton_t&
      rweight_here(automaton_t& res, const weight_t& w, Tag tag = {})
      {
        const auto& ws = *res->context().weightset();
        if (ws.is_zero(w))
          zero_here(res, tag);
        else if (!ws.is_one(w))
          for (auto t: final_transitions(res))
            res->rweight(t, w);
        res->properties().update(rweight_ftag{});
        return res;
      }

      /// Same as standard if res is standard, otherwise, general.
      static automaton_t&
      rweight_here(automaton_t& res, const weight_t& w, auto_tag = {})
      {
        if (is_standard(res))
          return rweight_here(res, w, standard_tag{});
        else
          return rweight_here(res, w, general_tag{});
      }

      /// Transform \a res into the empty automaton.
      static automaton_t&
      zero_here(automaton_t& res, general_tag)
      {
        res = make_fresh_automaton(res);
        return res;
      }

      /// Transform \a res into the (standard) empty automaton.
      static automaton_t&
      zero_here(automaton_t& res, standard_tag)
      {
        zero_here(res, general_tag{});
        res->set_initial(res->new_state());
        return res;
      }
    };
  }

  /*---------------------.
  | lweight(automaton).  |
  `---------------------*/

  /// In place left-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  Aut&
  lweight_here(const weight_t_of<Aut>& w, Aut& res, Tag tag = {})
  {
    return detail::standard_operations<Aut>::lweight_here(w, res, tag);
  }

  /// Left-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  auto
  lweight(const weight_t_of<Aut>& w, const Aut& aut, Tag tag = {})
    -> fresh_automaton_t_of<Aut>
  {
    auto res = copy(aut);
    lweight_here(w, res, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Left-product.
      template <Automaton Aut, typename Tag>
      automaton
      lweight_tag(const weight_t_of<Aut>& w, Aut& aut)
      {
        return ::vcsn::lweight_here(w, aut, Tag{});
      }

      /// Bridge.
      template <typename WeightSet, Automaton Aut, typename String>
      automaton
      lweight(const weight& weight, const automaton& aut,
                const std::string& algo)
      {
        const auto& a1 = aut->as<Aut>();
        const auto& w1 = weight->as<WeightSet>();
        // FIXME: this is hairy because there is no elegant means (so
        // far) to copy an automaton to a supertype, because the
        // incoming context is not automatically converted to the
        // supertype by vcsn::copy.
        auto ctx = make_context(*a1->labelset(),
                                join(w1.valueset(), *a1->weightset()));
        auto a2 = make_mutable_automaton(ctx);
        copy_into(a1, a2);
        using automaton_t = decltype(a2);
        auto w2 = ctx.weightset()->conv(w1.valueset(), w1.value());
        using weight_t = decltype(w2);
        static const auto map
          = getarg<std::function<automaton(const weight_t&, automaton_t&)>>
          {
            "left-multiply algorithm",
            {
              {"auto",     lweight_tag<automaton_t, auto_tag>},
              {"general",  lweight_tag<automaton_t, general_tag>},
              {"standard", lweight_tag<automaton_t, standard_tag>},
            }
          };
        return map[algo](w2, a2);
      }
    }
  }

  /*---------------------.
  | lweight(valueset).   |
  `---------------------*/

  template <typename ValueSet>
  auto
  lweight(const ValueSet& rs,
            const weight_t_of<ValueSet>& w,
            const typename ValueSet::value_t& r)
    -> decltype(rs.lweight(w, r)) // for SFINAE.
  {
    return rs.lweight(w, r);
  }

  /*----------------------.
  | lweight(expansion).   |
  `----------------------*/

  template <typename WeightSet, typename ExpSet>
  expansionset<expressionset<context<labelset_t_of<ExpSet>,
                                     join_t<WeightSet, weightset_t_of<ExpSet>>>>>
  join_weightset_expansionset(const WeightSet& ws,
                              const expansionset<ExpSet>& rs)
  {
    return make_expansionset(join_weightset_expressionset(ws,
                                                          rs.expressionset()));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (lweight).
      template <typename WeightSet, typename ExpansionSet>
      expansion
      lweight_expansion(const weight& weight, const expansion& exp)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpansionSet>();
        auto rs = join_weightset_expansionset(w1.valueset(), r1.valueset());
        auto w2
          = rs.expressionset().weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.valueset(), r1.value());
        return {rs, ::vcsn::lweight(rs, w2, r2)};
      }
    }
  }

  /*-----------------------.
  | lweight(expression).   |
  `-----------------------*/

  /// Join between an expressionset and a weightset.
  ///
  /// We must not perform a plain
  ///
  /// join(w1.weightset(), r1.expressionset())
  ///
  /// here.  Consider for instance
  ///
  /// expressionset<lal(abc), expressionset<law(xyz), b>>
  ///
  /// we would perform
  ///
  /// join(expressionset<law(xyz), b>,
  ///      expressionset<lal(abc), expressionset<law(xyz), b>>)
  ///
  /// i.e., a join of contexts which applies to both labelsets
  /// (here, join(lal(abc), "law(xyz)) = law(abcxyz)") and
  /// weightsets.  Here, the "expressionset<law(xyz), b>" must really
  /// be considered as a weightset, so compute the join of
  /// weightsets by hand, and leave the labelset alone.
  template <typename WeightSet, typename ExpSet>
  expressionset<context<labelset_t_of<ExpSet>,
                        join_t<WeightSet, weightset_t_of<ExpSet>>>>
  join_weightset_expressionset(const WeightSet& ws,
                               const ExpSet& rs)
  {
    auto ctx = make_context(*rs.labelset(), join(ws, *rs.weightset()));
    return make_expressionset(ctx, rs.identities());
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (lweight).
      template <typename WeightSet, typename ExpSet>
      expression
      lweight_expression(const weight& weight, const expression& exp)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpSet>();
        auto rs = join_weightset_expressionset(w1.valueset(), r1.valueset());
        auto w2 = rs.weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.valueset(), r1.value());
        return {rs, ::vcsn::lweight(rs, w2, r2)};
      }
    }
  }

  /*-----------------------.
  | lweight(polynomial).   |
  `-----------------------*/

  template <typename WeightSet, typename ExpSet>
  auto
  join_weightset_polynomialset(const WeightSet& ws,
                               const polynomialset<ExpSet>& ps)
  {
    return make_polynomialset(make_context(*ps.labelset(),
                                           join(ws, *ps.weightset())));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (lweight).
      template <typename WeightSet, typename PolynomialSet>
      polynomial
      lweight_polynomial(const weight& weight, const polynomial& poly)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& p1 = poly->as<PolynomialSet>();
        auto ps = join_weightset_polynomialset(w1.valueset(), p1.valueset());
        auto w2 = ps.weightset()->conv(w1.valueset(), w1.value());
        auto p2 = ps.conv(p1.valueset(), p1.value());
        return {ps, ::vcsn::lweight(ps, w2, p2)};
      }
    }
  }


  /*---------------------.
  | rweight(automaton).  |
  `---------------------*/

  /// In place right-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  Aut&
  rweight_here(Aut& res, const weight_t_of<Aut>& w, Tag tag = {})
  {
    return detail::standard_operations<Aut>::rweight_here(res, w, tag);
  }

  /// Right-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  fresh_automaton_t_of<Aut>
  rweight(const Aut& aut, const weight_t_of<Aut>& w, Tag tag = {})
  {
    auto res = copy(aut);
    rweight_here(res, w, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Right-product.
      template <Automaton Aut, typename Tag>
      automaton
      rweight_tag(Aut& aut, const weight_t_of<Aut>& w)
      {
        return ::vcsn::rweight_here(aut, w, Tag{});
      }

      /// Bridge.
      template <Automaton Aut, typename WeightSet, typename String>
      automaton
      rweight(const automaton& aut, const weight& weight,
                 const std::string& algo)
      {
        const auto& a1 = aut->as<Aut>();
        const auto& w1 = weight->as<WeightSet>();
        // FIXME: this is hairy because there is no elegant means (so
        // far) to copy an automaton to a supertype, because the
        // incoming context is not automatically converted to the
        // supertype by vcsn::copy.
        auto ctx = make_context(*a1->labelset(),
                                join(w1.valueset(), *a1->weightset()));
        auto a2 = make_mutable_automaton(ctx);
        copy_into(a1, a2);
        using automaton_t = decltype(a2);
        auto w2 = ctx.weightset()->conv(w1.valueset(), w1.value());
        using weight_t = decltype(w2);
        static const auto map
          = getarg<std::function<automaton(automaton_t&, const weight_t&)>>
          {
            "right-multiply algorithm",
            {
              {"auto",     rweight_tag<automaton_t, auto_tag>},
              {"general",  rweight_tag<automaton_t, general_tag>},
              {"standard", rweight_tag<automaton_t, standard_tag>},
            }
          };
        return map[algo](a2, w2);
      }
    }
  }

  /*---------------------.
  | rweight(valueset).   |
  `---------------------*/

  template <typename ValueSet>
  typename ValueSet::value_t
  rweight(const ValueSet& rs,
             const typename ValueSet::value_t& r,
             const weight_t_of<ValueSet>& w)
  {
    return rs.rweight(r, w);
  }

  /*----------------------.
  | rweight(expansion).   |
  `----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (rweight).
      template <typename ExpansionSet, typename WeightSet>
      expansion
      rweight_expansion(const expansion& exp, const weight& weight)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpansionSet>();
        auto rs = join_weightset_expansionset(w1.valueset(), r1.valueset());
        auto w2
          = rs.expressionset().weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.valueset(), r1.value());
        return {rs, ::vcsn::rweight(rs, r2, w2)};
      }
    }
  }

  /*-----------------------.
  | rweight(expression).   |
  `-----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (rweight).
      template <typename ExpSet, typename WeightSet>
      expression
      rweight_expression(const expression& exp, const weight& weight)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpSet>();
        auto rs = join_weightset_expressionset(w1.valueset(), r1.valueset());
        auto w2 = rs.weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.valueset(), r1.value());
        return {rs, ::vcsn::rweight(rs, r2, w2)};
      }
    }
  }

  /*-----------------------.
  | rweight(polynomial).   |
  `-----------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (rweight).
      template <typename PolynomialSet, typename WeightSet>
      polynomial
      rweight_polynomial(const polynomial& poly, const weight& weight)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& p1 = poly->as<PolynomialSet>();
        auto ps = join_weightset_polynomialset(w1.valueset(), p1.valueset());
        auto w2 = ps.weightset()->conv(w1.valueset(), w1.value());
        auto p2 = ps.conv(p1.valueset(), p1.value());
        return {ps, ::vcsn::rweight(ps, p2, w2)};
      }
    }
  }
}
