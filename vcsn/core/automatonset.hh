#pragma once

#include <vcsn/algos/complement.hh>
#include <vcsn/algos/complete.hh>
#include <vcsn/algos/compose.hh>
#include <vcsn/algos/conjunction.hh>
#include <vcsn/algos/left-mult.hh>
#include <vcsn/algos/multiply.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/algos/sum.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/context.hh>

namespace vcsn
{

  template <typename Context, typename Tag>
  class automatonset
  {
  public:
    using context_t = Context;
    using tag_t = Tag;
    using value_t = mutable_automaton<context_t>;
    using labelset_t = labelset_t_of<context_t>;
    using weightset_t = weightset_t_of<context_t>;
    using kind_t = typename context_t::kind_t;

    /// Lightweight state handle (or index).
    using state_t = unsigned;
    /// Lightweight transition handle (or index).
    using transition_t = unsigned;
    /// Transition label.
    using label_t = typename labelset_t::value_t;
    /// Transition weight.
    using weight_t = typename weightset_t::value_t;

    static symbol sname()
    {
      static auto res = symbol{"automatonset<" + context_t::sname()
                               + ", " + tag_t::sname() + '>'};
      return res;
    }

    automatonset(const context_t& c)
      : ctx_{c}
    {}

    /// Format the description of this automatonset.
    auto print_set(std::ostream& o, format fmt = {}) const
      -> std::ostream&
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\mathsf{Aut}[";
          ctx_.print_set(o, fmt);
          o << ", " << tag_t::sname() << ']';
          break;
        case format::sname:
          o << "automatonset<";
          ctx_.print_set(o, fmt);
          o << ", " << tag_t::sname() << '>';
          break;
        case format::text:
        case format::utf8:
          o << "Aut[";
          ctx_.print_set(o, fmt);
          o << ", " << tag_t::sname() << ']';
          break;
        case format::raw:
          assert(!"automatonset::print_set: invalid format: rat");
          break;
        }
      return o;
    }

    auto zero() -> value_t
    {
      auto res = make_mutable_automaton(ctx_);
      auto s = res->new_state();
      res->set_initial(s);
      return res;
    }

    auto one() -> value_t
    {
      auto res = make_mutable_automaton(ctx_);
      auto s = res->new_state();
      res->set_initial(s);
      res->set_final(s);
      return res;
    }

    auto atom(const label_t& l) const -> value_t
    {
      auto res = make_mutable_automaton(ctx_);
      auto s0 = res->new_state();
      res->set_initial(s0);
      auto s1 = res->new_state();
      res->new_transition(s0, s1, l);
      res->set_final(s1);
      return res;
    }

    static auto add(const value_t& l, const value_t& r) -> value_t
    {
      return ::vcsn::sum(l, r, tag_t{});
    }

    static auto mul(const value_t& l, const value_t& r) -> value_t
    {
      return ::vcsn::multiply(l, r, tag_t{});
    }

    /// Build a composition: `l @ r`.
    auto compose(const value_t& l, const value_t& r) const -> value_t
    {
      return ::vcsn::compose(l, r);
    }

    /// Build an conjunction product: `l & r`.
    auto conjunction(const value_t& l, const value_t& r) const -> value_t
    {
      namespace v = ::vcsn;
      return v::strip(v::coaccessible(v::conjunction(l, r)));
    }

    /// Build an infiltration product: `l &: r`.
    auto infiltration(const value_t& l, const value_t& r) const -> value_t
    {
      return ::vcsn::infiltration(l, r)->strip();
    }

    /// Build a shuffle product: `l : r`.
    auto shuffle(const value_t& l, const value_t& r) const -> value_t
    {
      return ::vcsn::shuffle(l, r)->strip();
    }

    /// Build a tuple: `e | f | ...`.
    template <typename... Value>
    auto tuple(Value&&... v) const -> value_t;

    /// Add a power operator: `e{n}`.
    auto power(const value_t& e, unsigned n) const -> value_t
    {
      return ::vcsn::multiply(e, n, tag_t{});
    }

    /// Build a left division: `l {\} r`.
    auto ldiv(const value_t& l, const value_t& r) const -> value_t
    {
      namespace v = ::vcsn;
      return detail::static_if<labelset_t::has_one()>
        ([this](const auto& l, const auto& r)
         {
           // We need a free labelset, but might need to restore the
           // context afterwards.
           auto a = v::strip(v::ldiv(v::proper(l), v::proper(r)));
           auto res = make_mutable_automaton(ctx_);
           copy_into(a, res);
           return res;
         },
#if defined __clang__ && __clang_major__ == 3 && __clang_minor__ < 6
         // Clang 3.5 requires that we name the argument.
         [this](auto&&... as) -> value_t
#else
         [this](auto&&...) -> value_t
#endif
         {
           raise("ldiv: ", *this, ": labelset must have a neutral");
         })
        (l, r);
    }

    /// Build a right division: `l {/} r`.
    auto rdiv(const value_t& l, const value_t& r) const -> value_t
    {
      namespace v = ::vcsn;
      return detail::static_if<labelset_t::has_one()>
        ([this](const auto& l, const auto& r)
         {
           // We need a free labelset, but might need to restore the
           // context afterwards.
           auto a = v::strip(v::rdiv(v::proper(l), v::proper(r)));
           auto res = make_mutable_automaton(ctx_);
           copy_into(a, res);
           return res;
         },
#if defined __clang__ && __clang_major__ == 3 && __clang_minor__ < 6
         // Clang 3.5 requires that we name the argument.
         [this](auto&&... as) -> value_t
#else
         [this](auto&&...) -> value_t
#endif
         {
           raise("ldiv: ", *this, ": labelset must have a neutral");
         })
        (l, r);
    }

    /// Add a star operator: `e*`.
    auto star(const value_t& e) const -> value_t
    {
      return ::vcsn::star(e, tag_t{});
    }

    /// Add a complement operator: `e{c}`.
    auto complement(const value_t& e) const -> value_t
    {
      namespace v = ::vcsn;
      auto a = v::complement(v::complete(v::determinize(v::proper(e))));
      // We need a free labelset, but might need to restore the
      // context afterwards.
      auto res = make_mutable_automaton(ctx_);
      copy_into(a, res);
      return res;
    }

    /// Add a transposition operator.
    auto transposition(const value_t& e) const -> value_t
    {
      return ::vcsn::transpose(e)->automaton();
    }

    /// Right-multiplication by a weight.
    auto rmul(const value_t& e, const weight_t& w) const -> value_t
    {
      return ::vcsn::right_mult(e, w, tag_t{});
    }

    /// Left-multiplication by a weight.
    auto lmul(const weight_t& w, const value_t& e) const -> value_t
    {
      return ::vcsn::left_mult(w, e, tag_t{});
    }

  protected:
    context_t ctx_;
  };
}
