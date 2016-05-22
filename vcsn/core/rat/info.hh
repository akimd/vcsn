#pragma once

#include <algorithm> // std::max
#include <iostream>

#include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    /// Gather information of the number of the different node types.
    ///
    /// \tparam ExpSet  the expressionset type.
    template <typename ExpSet>
    class info
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;
      using self_t = info;

      using context_t = context_t_of<expressionset_t>;
      using node_t = typename expressionset_t::node_t;

      /// For each node type, count its number of occurrences.
      void operator()(const node_t& v)
      {
        clear();
        v.accept(*this);
      }

      /// Entry point: compute info about \a v.
      void operator()(const std::shared_ptr<const node_t>& v)
      {
        operator()(*v);
      }

      void clear()
      {
        atom = 0;
        complement = 0;
        compose = 0;
        conjunction = 0;
        depth = 0;
        infiltration = 0;
        ldiv = 0;
        lweight = 0;
        one = 0;
        prod = 0;
        rweight = 0;
        shuffle = 0;
        star = 0;
        sum = 0;
        transposition = 0;
        tuple = 0;
        zero = 0;
      }

      /// Add the result from another info operator.
      template <typename OtherExpSet>
      info& operator+=(const info<OtherExpSet>& other)
      {
        atom += other.atom;
        complement += other.complement;
        compose += other.compose;
        conjunction += other.conjunction;
        depth = std::max(depth, other.depth);
        infiltration += other.infiltration;
        ldiv += other.ldiv;
        lweight += other.lweight;
        one += other.one;
        prod += other.prod;
        rweight += other.rweight;
        shuffle += other.shuffle;
        star += other.star;
        sum += other.sum;
        transposition += other.transposition;
        tuple += other.tuple;
        zero += other.zero;
        return *this;
      }

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "info"; }

      /// Depth of the tree.
      size_t depth = 0;

#define DEFINE(Type)                                    \
    public:                                             \
      size_t Type = 0;                                  \
    private:                                            \
      VCSN_RAT_VISIT(Type, v)

      DEFINE(atom)         { ++atom; (void) v; depth = 0; }
      DEFINE(complement)   { ++complement;   visit_(v); }
      DEFINE(compose)      { ++compose;      visit_(v); }
      DEFINE(conjunction)  { ++conjunction;  visit_(v); }
      DEFINE(infiltration) { ++infiltration; visit_(v); }
      DEFINE(ldiv)         { ++ldiv;         visit_(v); }
      DEFINE(lweight)      { ++lweight; v.sub()->accept(*this); ++depth; }
      DEFINE(one)          { ++one; (void) v; depth = 0; }
      DEFINE(prod)         { ++prod;         visit_(v);}
      DEFINE(rweight)      { ++rweight; v.sub()->accept(*this); ++depth; }
      DEFINE(shuffle)      { ++shuffle;      visit_(v); }
      DEFINE(star)         { ++star;         visit_(v); }
      DEFINE(sum)          { ++sum;          visit_(v);  }
      DEFINE(transposition){ ++transposition; visit_(v); }
      DEFINE(zero)         { ++zero; (void) v; depth = 0; }
#undef DEFINE

    private:
      template <type_t Type>
      using unary_t = rat::unary<Type, context_t>;

      /// Factor the visitation of unary nodes.
      template <rat::exp::type_t Type>
      void visit_(const unary_t<Type>& v)
      {
        v.sub()->accept(*this);
        ++depth;
      }

      template <type_t Type>
      using variadic_t = rat::variadic<Type, context_t>;

      /// Factor the visitation of variadic nodes.
      template <exp::type_t Type>
      void visit_(const variadic_t<Type>& v)
      {
        size_t d = 0;
        for (const auto& c: v)
          {
            c->accept(*this);
            d = std::max(d, depth);
          }
        depth = d + 1;
      }

      /*---------.
      | tuple.   |
      `---------*/

    public:
      /// Number of tuple operators.
      size_t tuple = 0;
      using tuple_t = typename super_t::tuple_t;

    private:
      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// Info about tape I.
        template <size_t I>
        void info_(const tuple_t& v)
        {
          using expset_t = typename expressionset_t::template project_t<I>;
          visitor_ += make_info<expset_t>(std::get<I>(v.sub()));
        }

        /// Info all the tapes.
        template <size_t... I>
        void info_(const tuple_t& v, detail::index_sequence<I...>)
        {
          using swallow = int[];
          (void) swallow
          {
            (info_<I>(v),
             0)...
          };
        }

        /// Entry point.
        void operator()(const tuple_t& v)
        {
          info_(v, labelset_t_of<context_t>::indices);
          ++visitor_.tuple;
          ++visitor_.depth;
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
    };

    template <typename ExpSet>
    info<ExpSet> make_info(const typename ExpSet::value_t& r)
    {
      auto s = info<ExpSet>{};
      s(r);
      return s;
    }
  } // namespace rat
} // namespace vcsn
