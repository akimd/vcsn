#pragma once

#include <set>

#include <vcsn/core/rat/visitor.hh>
#include <vcsn/core/automatonset.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{

  /*-------------------------.
  | inductive(expression).   |
  `-------------------------*/

  namespace rat
  {
    /// Build an automaton by induction from an expression.
    ///
    /// \tparam Aut      the type of the generated automaton.
    /// \tparam ExpSet   the expressionset.
    /// \tparam Tag      the nature of the operations (standard_tag, etc.).
    template <Automaton Aut, typename ExpSet, typename Tag>
    class inductive_visitor
      : public ExpSet::const_visitor
    {
    public:
      using automaton_t = Aut;
      using expressionset_t = ExpSet;
      using tag_t = Tag;

      using automatonset_t = automatonset<context_t_of<automaton_t>, tag_t>;
      using expression_t = typename expressionset_t::value_t;
      using weightset_t = weightset_t_of<expressionset_t>;
      using weight_t = weight_t_of<expressionset_t>;
      using state_t = state_t_of<automaton_t>;

      using super_t = typename expressionset_t::const_visitor;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "inductive"; }

      inductive_visitor(const expressionset_t& rs)
        : as_{automatonset_t{rs.context()}}
      {}

      automaton_t operator()(const expression_t& v)
      {
        v->accept(*this);
        return res_;
      }

    private:
      automaton_t recurse(const expression_t& v)
      {
        v->accept(*this);
        return res_;
      }

      using tuple_t = typename super_t::tuple_t;
      virtual void visit(const tuple_t&, std::true_type) override
      {
        raise(me(), ": tuple is not supported");
      }

      VCSN_RAT_VISIT(zero,)
      {
        res_ = as_.zero();
      }

      VCSN_RAT_VISIT(one,)
      {
        res_ = as_.one();
      }

      VCSN_RAT_VISIT(atom, e)
      {
        res_ = as_.atom(e.value());
      }

      VCSN_RAT_VISIT(conjunction, e)
      {
        auto res = recurse(e.head());
        for (const auto& c: e.tail())
          res = as_.conjunction(res, recurse(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(infiltration, e)
      {
        auto res = recurse(e.head());
        for (const auto& c: e.tail())
          res = as_.infiltration(res, recurse(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(ldiv, e)
      {
        auto res = recurse(e.head());
        for (const auto& c: e.tail())
          res = as_.ldiv(res, recurse(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(shuffle, e)
      {
        auto res = recurse(e.head());
        for (const auto& c: e.tail())
          res = as_.shuffle(res, recurse(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(sum, e)
      {
        auto res = recurse(e.head());
        for (const auto& c: e.tail())
          res = as_.add(res, recurse(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(prod, e)
      {
        auto res = recurse(e.head());
        for (const auto& c: e.tail())
          res = as_.mul(res, recurse(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(star, e)
      {
        res_ = as_.star(recurse(e.sub()));
      }

      VCSN_RAT_VISIT(complement, e)
      {
        res_ = as_.complement(recurse(e.sub()));
      }

      VCSN_RAT_VISIT(transposition, e)
      {
        res_ = as_.transposition(recurse(e.sub()));
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        res_ = as_.lmul(e.weight(), recurse(e.sub()));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        res_ = as_.rmul(recurse(e.sub()), e.weight());
      }

    private:
      automatonset_t as_;
      automaton_t res_ = nullptr;
    };
  } // rat::


  /// Build a inductive automaton from an expression.
  ///
  /// \tparam Aut      the type of the generated automaton.
  /// \tparam ExpSet   the expressionset.
  /// \tparam Tag      the nature of the operations (standard_tag, etc.).
  template <Automaton Aut, typename ExpSet, typename Tag>
  Aut
  inductive(const ExpSet& rs, const typename ExpSet::value_t& r,
            Tag = {})
  {
    auto ind = rat::inductive_visitor<Aut, ExpSet, Tag>{rs};
    return ind(r);
  }

  namespace dyn
  {
    namespace detail
    {
      /// Helper function to facilitate dispatch below.
      template <typename ExpSet, typename Tag>
      automaton inductive_tag_(const ExpSet& rs,
                               const typename ExpSet::value_t& r)
      {
        using automaton_t = vcsn::mutable_automaton<context_t_of<ExpSet>>;
        return ::vcsn::inductive<automaton_t>(rs, r, Tag{});
      }

      /// Bridge.
      template <typename ExpSet, typename String>
      automaton
      inductive(const expression& exp, const std::string& algo)
      {
        // FIXME: So far, there is a single implementation of expressions,
        // but we should actually be parameterized by its type too.
        using expressionset_t = ExpSet;
        using expression_t = typename expressionset_t::value_t;
        using automaton_t
          = vcsn::mutable_automaton<context_t_of<expressionset_t>>;
        const auto& e = exp->as<expressionset_t>();

        using fn_t
          = auto(const expressionset_t&, const expression_t&) -> automaton;
        static const auto map = getarg<std::function<fn_t>>
          {
            "inductive algorithm",
            {
              {"auto",     "standard"},
              // {"general",  inductive_tag_<ExpSet, general_tag>},
              {"standard", inductive_tag_<ExpSet, standard_tag>},
            }
          };
        return map[algo](e.valueset(), e.value());
      }
    }
  }
}
