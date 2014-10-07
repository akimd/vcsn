#ifndef VCSN_CORE_RAT_RATEXP_HXX_
# define VCSN_CORE_RAT_RATEXP_HXX_

# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

#define DEFINE_CTOR(Node)                       \
    template <typename Label, typename Weight>  \
    inline                                      \
    Node<Label, Weight>::Node

#define DEFINE(Node)                            \
    template <typename Label, typename Weight>  \
    inline                                      \
    auto                                        \
    Node<Label, Weight>

    /*-------.
    | atom.  |
    `-------*/

    DEFINE_CTOR(atom)(const label_t& value)
      : value_(value)
    {}

    DEFINE(atom)::accept(typename super_type::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(atom)::value() const
      -> const label_t&
    {
      return value_;
    }

# undef DEFINE_CTOR
# undef DEFINE


#define DEFINE_CTOR(Node)                                       \
    template <type_t Type, typename Label, typename Weight>     \
    inline                                                      \
    Node<Type, Label, Weight>::Node

#define DEFINE(Node)                                         \
    template <type_t Type, typename Label, typename Weight>  \
    inline                                                   \
    auto                                                     \
    Node<Type, Label, Weight>


    /*-----------.
    | variadic.  |
    `-----------*/

    DEFINE_CTOR(variadic)(const values_t& ns)
      : sub_(ns)
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

    DEFINE(variadic)::rbegin() const
      -> const_reverse_iterator
    {
      return sub_.begin();
    }

    DEFINE(variadic)::rend() const
      -> const_reverse_iterator
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

    DEFINE(variadic)::accept(typename super_type::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    /*---------.
    | weight.  |
    `---------*/

    DEFINE_CTOR(weight_node)(const weight_t& weight, value_t sub)
      : sub_(sub)
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

    DEFINE(weight_node)::set_weight(const weight_t& w)
      -> void
    {
      weight_ = w;
    }

    DEFINE(weight_node)::accept(typename super_type::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    /*--------.
    | unary.  |
    `--------*/

    DEFINE_CTOR(unary)(value_t sub)
      : sub_(sub)
    {}

    DEFINE(unary)::sub() const
      -> const value_t
    {
      return sub_;
    }

    DEFINE(unary)::accept(typename super_type::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    /*-----------.
    | constant.  |
    `-----------*/

    DEFINE(constant)::accept(typename super_type::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

# undef DEFINE_CTOR
# undef DEFINE

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_RATEXP_HXX_
