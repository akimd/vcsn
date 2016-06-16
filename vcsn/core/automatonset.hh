#pragma once

#include <vcsn/algos/complement.hh>
#include <vcsn/algos/complete.hh>
#include <vcsn/algos/compose.hh>
#include <vcsn/algos/conjunction.hh>
#include <vcsn/algos/weight.hh>
#include <vcsn/algos/multiply.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/algos/add.hh>
#include <vcsn/algos/tuple-automaton.hh>
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

    auto zero() const -> value_t
    {
      auto res = make_mutable_automaton(ctx_);
      auto s = res->new_state();
      res->set_initial(s);
      return res;
    }

    auto one() const -> value_t
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
      return ::vcsn::add(l, r, tag_t{});
    }

    static auto mul(const value_t& l, const value_t& r) -> value_t
    {
      return ::vcsn::multiply(l, r, tag_t{});
    }

    /// Build a composition: `l @ r`.
    template <typename Ctx = context_t>
    auto compose(const value_t& l, const value_t& r) const
      -> std::enable_if_t<are_composable<Ctx, Ctx>{}, value_t>
    {
      return ::vcsn::strip(::vcsn::compose(l, r));
    }

    /// Build an conjunction product: `l & r`.
    auto conjunction(const value_t& l, const value_t& r) const -> value_t
    {
      namespace v = ::vcsn;
      auto res = v::strip(v::coaccessible(v::conjunction(l, r)));
      // Preserve standardness.
      return v::is_empty(res) ? zero() : res;
    }

    /// Build an infiltration product: `l &: r`.
    auto infiltrate(const value_t& l, const value_t& r) const -> value_t
    {
      return ::vcsn::infiltrate(l, r)->strip();
    }

    /// Build a shuffle product: `l : r`.
    auto shuffle(const value_t& l, const value_t& r) const -> value_t
    {
      return ::vcsn::shuffle(l, r)->strip();
    }

    /// Build a tuple: `e | f | ...`.
    template <typename... Value>
    auto tuple(Value&&... v) const -> value_t
    {
      return ::vcsn::tuple(v...)->strip();
    }

    /// Add a power operator: `e{n}`.
    auto power(const value_t& e, unsigned n) const -> value_t
    {
      return ::vcsn::multiply(e, n, tag_t{});
    }

    /// Build a left division: `l {\} r`.
    auto ldiv(const value_t& l, const value_t& r) const -> value_t
    {
      namespace v = ::vcsn;
      // We need a free labelset, but proper automata.
      auto lhs = make_nullable_automaton(ctx_);
      copy_into(v::proper(l), lhs);
      auto rhs = make_nullable_automaton(ctx_);
      copy_into(v::proper(r), rhs);

      auto res = make_mutable_automaton(ctx_);
      // ldiv might introduce several initial states, e.g. a? \ a.
      copy_into(v::proper(v::accessible(v::standard(v::ldiv(lhs, rhs)))), res);
      return v::is_empty(res) ? zero() : res;
    }

    /// Build a right division: `l {/} r`.
    auto rdiv(const value_t& l, const value_t& r) const -> value_t
    {
      namespace v = ::vcsn;
      // We need a free labelset, but proper automata.
      auto lhs = make_nullable_automaton(ctx_);
      copy_into(v::proper(l), lhs);
      auto rhs = make_nullable_automaton(ctx_);
      copy_into(v::proper(r), rhs);

      auto res = make_mutable_automaton(ctx_);
      copy_into(v::standard(v::proper(v::coaccessible(v::rdiv(lhs, rhs)))),
                res);
      return v::is_empty(res) ? zero() : res;
    }

    /// Add a star operator: `e*`.
    auto star(const value_t& e) const -> value_t
    {
      return ::vcsn::star(e, tag_t{});
    }

    /// Add a complement operator: `e{c}`.
    auto complement(const value_t& e, std::true_type) const -> value_t
    {
      namespace v = ::vcsn;
      // The automaton for e does not have spontaneous transitions,
      // but we call proper because determinize needs the labelset to
      // be free.  But then the result is free too, we might have to
      // restore the context.
      auto a =
        v::coaccessible(v::complement(v::complete(v::determinize(v::proper(e)))));
      if (v::is_empty(a))
        return zero();
      else
      {
        auto res = make_mutable_automaton(ctx_);
        copy_into(a, res);
        return res;
      }
    }

    /// Add a complement operator: `e{c}`.
    auto complement(const value_t& e, std::false_type) const -> value_t
    {
      raise("inductive: cannot complement on multitape: ", ctx_);
    }

    /// Add a complement operator: `e{c}`.
    auto complement(const value_t& e) const -> value_t
    {
      return complement(e, bool_constant<!detail::is_multitape<context_t>{}>{});
    }

    /// Add a transposition operator.
    auto transposition(const value_t& e) const -> value_t
    {
      return ::vcsn::standard(::vcsn::transpose(e)->automaton());
    }

    /// Right-multiplication by a weight.
    auto rweight(const value_t& e, const weight_t& w) const -> value_t
    {
      return ::vcsn::rweight(e, w, tag_t{});
    }

    /// Left-multiplication by a weight.
    auto lweight(const weight_t& w, const value_t& e) const -> value_t
    {
      return ::vcsn::lweight(w, e, tag_t{});
    }

  protected:
    context_t ctx_;
  };
}
