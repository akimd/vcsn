#pragma once

#include <vcsn/core/rat/expression.hh>
#include <vcsn/core/rat/visitor.hh>

namespace vcsn::rat
{

#define DEFINE_CTOR(Node)                       \
  template <typename Context>                   \
  Node<Context>::Node

#define DEFINE(Node)                            \
  template <typename Context>                   \
  auto                                          \
  Node<Context>

  /*--------.
  | atom.   |
  `--------*/

  DEFINE_CTOR(atom)(const label_t& value)
    : value_(value)
  {}

  DEFINE(atom)::accept(typename super_t::const_visitor& v) const
    -> void
  {
    v.visit(*this);
  }

  DEFINE(atom)::value() const
    -> const label_t&
  {
    return value_;
  }

  /*--------.
  | name.   |
  `--------*/

  DEFINE_CTOR(name)(value_t sub, symbol name)
    : sub_{std::move(sub)}
    , name_{name}
  {}

  DEFINE(name)::sub() const
    -> const value_t
  {
    return sub_;
  }

  DEFINE(name)::name_get() const
    -> symbol
  {
    return name_;
  }

  DEFINE(name)::accept(typename super_t::const_visitor& v) const
    -> void
  {
    v.visit(*this);
  }

#undef DEFINE_CTOR
#undef DEFINE


#define DEFINE_CTOR(Node)                       \
  template <type_t Type, typename Context>      \
  Node<Type, Context>::Node

#define DEFINE(Node)                            \
  template <type_t Type, typename Context>      \
  auto                                          \
  Node<Type, Context>


  /*------------.
  | variadic.   |
  `------------*/

  DEFINE_CTOR(variadic)(values_t ns)
    : sub_(std::move(ns))
  {}


  DEFINE(variadic)::begin() const
    -> const_iterator
  {
    return sub_.begin();
  }

  DEFINE(variadic)::end() const
    -> const_iterator
  {
    return sub_.end();
  }

  DEFINE(variadic)::size() const
    -> size_t
  {
    return sub_.size();
  }

  DEFINE(variadic)::operator[](size_t n) const
    -> const value_t
  {
    return sub_[n];
  }

  DEFINE(variadic)::head() const
    -> const value_t
  {
    return *begin();
  }

  DEFINE(variadic)::back() const
    -> const value_t
  {
    return sub_.back();
  }

  DEFINE(variadic)::tail() const
    -> decltype(boost::make_iterator_range(*this, 1, 0))
  {
    return boost::make_iterator_range(*this, 1, 0);
  }

  DEFINE(variadic)::subs() const
    -> values_t
  {
    return sub_;
  }

  DEFINE(variadic)::accept(typename super_t::const_visitor& v) const
    -> void
  {
    v.visit(*this);
  }

  /*---------.
  | unary.   |
  `---------*/

  DEFINE_CTOR(unary)(value_t sub)
    : sub_(sub)
  {}

  DEFINE(unary)::sub() const
    -> const value_t
  {
    return sub_;
  }

  DEFINE(unary)::accept(typename super_t::const_visitor& v) const
    -> void
  {
    v.visit(*this);
  }

  /*----------.
  | weight.   |
  `----------*/

  DEFINE_CTOR(weight_node)(weight_t weight, value_t sub)
    : sub_(std::move(sub))
    , weight_(weight)
  {}

  DEFINE(weight_node)::sub() const
    -> const value_t
  {
    return sub_;
  }

  DEFINE(weight_node)::weight() const
    -> const weight_t&
  {
    return weight_;
  }

  DEFINE(weight_node)::set_weight(weight_t w)
    -> void
  {
    weight_ = w;
  }

  DEFINE(weight_node)::accept(typename super_t::const_visitor& v) const
    -> void
  {
    v.visit(*this);
  }


  /*------------.
  | constant.   |
  `------------*/

  DEFINE(constant)::accept(typename super_t::const_visitor& v) const
    -> void
  {
    v.visit(*this);
  }

#undef DEFINE_CTOR
#undef DEFINE

} // namespace rat
