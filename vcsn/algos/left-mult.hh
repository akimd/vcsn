#pragma once

#include <vcsn/algos/copy.hh>
#include <vcsn/algos/standard.hh>
#include <vcsn/algos/tags.hh>
#include <vcsn/core/rat/expressionset.hh>
#include <vcsn/core/rat/expansionset.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/dyn/automaton.hh> // dyn::make_automaton
#include <vcsn/dyn/expression.hh>
#include <vcsn/dyn/expansion.hh>
#include <vcsn/dyn/weight.hh>
#include <vcsn/misc/getargs.hh>

namespace vcsn
{
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
      left_mult_here(const weight_t& w, automaton_t& res, general_tag tag)
      {
        const auto& ws = *res->context().weightset();
        if (ws.is_zero(w))
          zero_here(res, tag);
        else if (!ws.is_one(w))
          for (auto t: initial_transitions(res))
            res->lmul_weight(t, w);
        return res;
      }

      /// Standard-preserving left-multiplication by a weight.
      static automaton_t&
      left_mult_here(const weight_t& w, automaton_t& res, standard_tag tag)
      {
        require(is_standard(res), __func__, ": automaton must be standard");
        const auto& ws = *res->context().weightset();
        if (ws.is_zero(w))
          zero_here(res, tag);
        else if (!ws.is_one(w))
          {
            state_t initial = res->dst_of(initial_transitions(res).front());
            for (auto t: all_out(res, initial))
              res->lmul_weight(t, w);
          }
        return res;
      }

      /// Same as standard if res is standard, otherwise, general.
      static automaton_t&
      left_mult_here(const weight_t& w, automaton_t& res, auto_tag = {})
      {
        if (is_standard(res))
          return left_mult_here(w, res, standard_tag{});
        else
          return left_mult_here(w, res, general_tag{});
      }

      /// Right-multiplication of any automaton by a weight.
      template <typename Tag = general_tag>
      static automaton_t&
      right_mult_here(automaton_t& res, const weight_t& w, Tag tag = {})
      {
        const auto& ws = *res->context().weightset();
        if (ws.is_zero(w))
          zero_here(res, tag);
        else if (!ws.is_one(w))
          for (auto t: final_transitions(res))
            res->rmul_weight(t, w);
        return res;
      }

      /// Same as standard if res is standard, otherwise, general.
      static automaton_t&
      right_mult_here(automaton_t& res, const weight_t& w, auto_tag = {})
      {
        if (is_standard(res))
          return right_mult_here(res, w, standard_tag{});
        else
          return right_mult_here(res, w, general_tag{});
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

  /*-----------------------.
  | left-mult(automaton).  |
  `-----------------------*/

  /// In place left-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  Aut&
  left_mult_here(const weight_t_of<Aut>& w, Aut& res, Tag tag = {})
  {
    return detail::standard_operations<Aut>::left_mult_here(w, res, tag);
  }

  /// Left-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  auto
  left_mult(const weight_t_of<Aut>& w, const Aut& aut, Tag tag = {})
    -> fresh_automaton_t_of<Aut>
  {
    auto res = copy(aut);
    left_mult_here(w, res, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Left-product.
      template <Automaton Aut, typename Tag>
      automaton
      left_mult_tag(const weight_t_of<Aut>& w, Aut& aut)
      {
        return ::vcsn::left_mult_here(w, aut, Tag{});
      }

      /// Bridge.
      template <typename WeightSet, Automaton Aut, typename String>
      automaton
      left_mult(const weight& weight, const automaton& aut,
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
              {"auto",     left_mult_tag<automaton_t, auto_tag>},
              {"general",  left_mult_tag<automaton_t, general_tag>},
              {"standard", left_mult_tag<automaton_t, standard_tag>},
            }
          };
        return map[algo](w2, a2);
      }
    }
  }

  /*-----------------------.
  | left-mult(valueset).   |
  `-----------------------*/

  template <typename ValueSet>
  auto
  left_mult(const ValueSet& rs,
            const weight_t_of<ValueSet>& w,
            const typename ValueSet::value_t& r)
    -> decltype(rs.lmul(w, r)) // for SFINAE.
  {
    return rs.lmul(w, r);
  }

  /*------------------------.
  | left-mult(expansion).   |
  `------------------------*/

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
      /// Bridge (left_mult).
      template <typename WeightSet, typename ExpansionSet>
      expansion
      left_mult_expansion(const weight& weight, const expansion& exp)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpansionSet>();
        auto rs
          = join_weightset_expansionset(w1.valueset(), r1.expansionset());
        auto w2
          = rs.expressionset().weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.expansionset(), r1.expansion());
        return make_expansion(rs, ::vcsn::left_mult(rs, w2, r2));
      }
    }
  }

  /*-------------------------.
  | left-mult(expression).   |
  `-------------------------*/

  /// Join between an expressionset and a weightset.
  ///
  /// We must not perform a plain
  ///
  /// join(w1.valueset(), r1.expressionset())
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
      /// Bridge (left_mult).
      template <typename WeightSet, typename ExpSet>
      expression
      left_mult_expression(const weight& weight, const expression& exp)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpSet>();
        auto rs
          = join_weightset_expressionset(w1.valueset(), r1.expressionset());
        auto w2 = rs.weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.expressionset(), r1.expression());
        return make_expression(rs,
                               ::vcsn::left_mult(rs, w2, r2));
      }
    }
  }

  /*------------------------.
  | right-mult(automaton).  |
  `------------------------*/

  /// In place right-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  Aut&
  right_mult_here(Aut& res, const weight_t_of<Aut>& w, Tag tag = {})
  {
    return detail::standard_operations<Aut>::right_mult_here(res, w, tag);
  }

  /// Right-multiplication of an automaton by a weight.
  template <Automaton Aut, typename Tag = auto_tag>
  fresh_automaton_t_of<Aut>
  right_mult(const Aut& aut, const weight_t_of<Aut>& w, Tag tag = {})
  {
    auto res = copy(aut);
    right_mult_here(res, w, tag);
    return res;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Right-product.
      template <Automaton Aut, typename Tag>
      automaton
      right_mult_tag(Aut& aut, const weight_t_of<Aut>& w)
      {
        return ::vcsn::right_mult_here(aut, w, Tag{});
      }

      /// Bridge.
      template <Automaton Aut, typename WeightSet, typename String>
      automaton
      right_mult(const automaton& aut, const weight& weight,
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
              {"auto",     right_mult_tag<automaton_t, auto_tag>},
              {"general",  right_mult_tag<automaton_t, general_tag>},
              {"standard", right_mult_tag<automaton_t, standard_tag>},
            }
          };
        return map[algo](a2, w2);
      }
    }
  }

  /*------------------------.
  | right-mult(valueset).   |
  `------------------------*/

  template <typename ValueSet>
  typename ValueSet::value_t
  right_mult(const ValueSet& rs,
             const typename ValueSet::value_t& r,
             const weight_t_of<ValueSet>& w)
  {
    return rs.rmul(r, w);
  }

  /*-------------------------.
  | right-mult(expansion).   |
  `-------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (right_mult).
      template <typename ExpansionSet, typename WeightSet>
      expansion
      right_mult_expansion(const expansion& exp, const weight& weight)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpansionSet>();
        auto rs
          = join_weightset_expansionset(w1.valueset(), r1.expansionset());
        auto w2
          = rs.expressionset().weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.expansionset(), r1.expansion());
        return make_expansion(rs, ::vcsn::right_mult(rs, r2, w2));
      }
    }
  }

  /*--------------------------.
  | right-mult(expression).   |
  `--------------------------*/

  namespace dyn
  {
    namespace detail
    {
      /// Bridge (right_mult).
      template <typename ExpSet, typename WeightSet>
      expression
      right_mult_expression(const expression& exp, const weight& weight)
      {
        const auto& w1 = weight->as<WeightSet>();
        const auto& r1 = exp->as<ExpSet>();
        auto rs
          = join_weightset_expressionset(w1.valueset(), r1.expressionset());
        auto w2 = rs.weightset()->conv(w1.valueset(), w1.value());
        auto r2 = rs.conv(r1.expressionset(), r1.expression());
        return make_expression(rs, ::vcsn::right_mult(rs, r2, w2));
      }
    }
  }
}
