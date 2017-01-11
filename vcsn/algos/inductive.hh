#pragma once

#include <vcsn/core/automatonset.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/value.hh>

namespace vcsn
{
  /*----------------.
  | Function tag.   |
  `----------------*/

  CREATE_FUNCTION_TAG(inductive);

  /*-------------------------.
  | inductive(expression).   |
  `-------------------------*/

  /// Build a inductive automaton from an expression.
  ///
  /// \tparam Aut      the type of the generated automaton.
  /// \tparam ExpSet   the expressionset.
  /// \tparam Tag      the nature of the operations (standard_tag, etc.).
  template <Automaton Aut, typename ExpSet, typename Tag>
  Aut
  inductive(const ExpSet& rs, const typename ExpSet::value_t& r,
            Tag = {});

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
      using self_t = inductive_visitor;

      using context_t = context_t_of<expressionset_t>;
      using automatonset_t = automatonset<context_t_of<automaton_t>, tag_t>;
      using expression_t = typename expressionset_t::value_t;
      using labelset_t = labelset_t_of<context_t>;
      using weightset_t = weightset_t_of<context_t>;
      using weight_t = weight_t_of<context_t>;
      using state_t = state_t_of<automaton_t>;

      using super_t = typename expressionset_t::const_visitor;

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "inductive"; }

      inductive_visitor(const expressionset_t& rs)
        : rs_{rs}
        , as_{rs_.context()}
      {}

      automaton_t operator()(const expression_t& v)
      {
        try
          {
            v->accept(*this);
            res_->properties().update(inductive_ftag{});
            return std::move(res_);
          }
        catch (const std::runtime_error& e)
          {
            raise(e, "  while computing inductive of: ", to_string(rs_, v));
          }
      }

    private:
      automaton_t recurse(const expression_t& v)
      {
        v->accept(*this);
        return std::move(res_);
      }

      using tuple_t = typename super_t::tuple_t;
      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// One tape.
        template <size_t I>
        auto tape_(const tuple_t& v)
        {
          const auto& rs = project<I>(visitor_.rs_);
          using automaton_t =
            vcsn::mutable_automaton<context_t_of<decltype(rs)>>;
          return ::vcsn::inductive<automaton_t>(rs,
                                                std::get<I>(v.sub()),
                                                tag_t{});
        }

        /// Sum of sizes for all tapes.
        template <size_t... I>
        auto tape_(const tuple_t& v, detail::index_sequence<I...>)
        {
          return visitor_.as_.tuple(tape_<I>(v)...);
        }

        /// Entry point.
        auto operator()(const tuple_t& v)
        {
          visitor_.res_ = tape_(v, labelset_t::indices);
        }
        self_t& visitor_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        void operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        self_t& visitor_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        visit_tuple<>{*this}(v);
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

      /// The type of the AutSet::compose() member function.
      template <Automaton AutSet>
      using compose_mem_fn_t =
        decltype(std::declval<AutSet>()
                 .compose(std::declval<typename AutSet::value_t>(),
                          std::declval<typename AutSet::value_t>()));

      /// Whether AutSet features a compose() member function.
      template <typename AutSet>
      using has_compose_mem_fn = detail::detect<AutSet, compose_mem_fn_t>;


      VCSN_RAT_VISIT(compose, e)
      {
        detail::static_if<has_compose_mem_fn<automatonset_t>{}>
          ([this](const auto& e)
           {
             auto res = recurse(e.head());
             for (const auto& c: e.tail())
               res = as_.compose(res, recurse(c));
             res_ = std::move(res);
           },
           [](const auto&)
           {
             raise("compose: context is not composable");
           })
            (e);
      }

      VCSN_RAT_VISIT(conjunction, e)
      {
        detail::static_if<labelset_t::is_letterized()>
          ([this](const auto& e)
           {
             auto res = recurse(e.head());
             for (const auto& c: e.tail())
               res = as_.conjunction(res, recurse(c));
             res_ = std::move(res);
           },
           [this](const auto&)
           {
             raise("conjunction: labelset must be letterized: ",
                   *rs_.labelset());
           })
          (e);
      }

      VCSN_RAT_VISIT(infiltrate, e)
      {
        detail::static_if<labelset_t::is_letterized()>
          ([this](const auto& e)
           {
             auto res = recurse(e.head());
             for (const auto& c: e.tail())
               res = as_.infiltrate(res, recurse(c));
             res_ = std::move(res);
           },
           [this](const auto&)
           {
             raise("infiltrate: labelset must be letterized: ",
                   *rs_.labelset());
           })
          (e);
      }

      VCSN_RAT_VISIT(ldivide, e)
      {
        detail::static_if<labelset_t::is_letterized()>
          ([this](const auto& e)
           {
             auto res = recurse(e.head());
             for (const auto& c: e.tail())
               res = as_.ldivide(res, recurse(c));
             res_ = std::move(res);
           },
           [this](const auto&)
           {
             raise("ldivide: labelset must be letterized: ",
                   *rs_.labelset());
           })
          (e);
      }

      VCSN_RAT_VISIT(shuffle, e)
      {
        detail::static_if<labelset_t::is_letterized()>
          ([this](const auto& e)
           {
             auto res = recurse(e.head());
             for (const auto& c: e.tail())
               res = as_.shuffle(res, recurse(c));
             res_ = std::move(res);
           },
           [this](const auto&)
           {
             raise("shuffle: labelset must be letterized: ",
                   *rs_.labelset());
           })
          (e);
      }

      VCSN_RAT_VISIT(add, e)
      {
        auto res = recurse(e.head());
        for (const auto& c: e.tail())
          res = as_.add(res, recurse(c));
        res_ = std::move(res);
      }

      VCSN_RAT_VISIT(mul, e)
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
        detail::static_if<labelset_t::is_letterized()>
          ([this](const auto& e)
           {
             // Complement checks if it is really free.
             res_ = as_.complement(recurse(e.sub()));
           },
           [this](const auto&)
           {
             raise("complement: labelset must be letterized: ",
                   *rs_.labelset());
           })
          (e);
      }

      VCSN_RAT_VISIT(transposition, e)
      {
        res_ = as_.transposition(recurse(e.sub()));
      }

      VCSN_RAT_VISIT(lweight, e)
      {
        res_ = as_.lweight(e.weight(), recurse(e.sub()));
      }

      VCSN_RAT_VISIT(rweight, e)
      {
        res_ = as_.rweight(recurse(e.sub()), e.weight());
      }

    private:
      expressionset_t rs_;
      automatonset_t as_;
      automaton_t res_ = nullptr;
    };
  } // rat::


  template <Automaton Aut, typename ExpSet, typename Tag>
  Aut
  inductive(const ExpSet& rs, const typename ExpSet::value_t& r, Tag)
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
