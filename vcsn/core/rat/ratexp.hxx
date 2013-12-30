#ifndef VCSN_CORE_RAT_RATEXP_HXX_
# define VCSN_CORE_RAT_RATEXP_HXX_

# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    inline
    exp::~exp()
    {}


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
    | node.  |
    `-------*/

    DEFINE(node)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };

    /*--------.
    | inner.  |
    `--------*/

    DEFINE(inner)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };


    /*-------.
    | leaf.  |
    `-------*/

    DEFINE(leaf)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };


    /*-------.
    | star.  |
    `-------*/

    DEFINE_CTOR(star)(value_t sub_exp)
      : sub_exp_(sub_exp)
    {}

    DEFINE(star)::sub() const
      -> const value_t
    {
      return sub_exp_;
    }


    DEFINE(star)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(star)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };


    /*-------.
    | atom.  |
    `-------*/

    DEFINE_CTOR(atom)(const label_t& value)
      : value_(value)
    {}

    DEFINE(atom)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(atom)::value() const
      -> const label_t&
    {
      return value_;
    }

    DEFINE(atom)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };

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


    /*-------.
    | nary.  |
    `-------*/

    DEFINE_CTOR(nary)(const ratexps_t& ns)
      : sub_ratexp_(ns)
    {
    }


    DEFINE(nary)::begin() const
      -> const_iterator
    {
      return sub_ratexp_.begin();
    }

    DEFINE(nary)::end() const
      -> const_iterator
    {
      return sub_ratexp_.end();
    }

    DEFINE(nary)::rbegin() const
      -> const_reverse_iterator
    {
      return sub_ratexp_.begin();
    }

    DEFINE(nary)::rend() const
      -> const_reverse_iterator
    {
      return sub_ratexp_.end();
    }

    DEFINE(nary)::size() const
      -> size_t
    {
      return sub_ratexp_.size();
    }

    DEFINE(nary)::operator[](size_t n) const
      -> const value_t
    {
      return sub_ratexp_[n];
    }

    DEFINE(nary)::head() const
      -> const value_t
    {
      return *begin();
    }

    DEFINE(nary)::tail() const
      -> decltype(boost::make_iterator_range(*this, 1, 0))
    {
      return boost::make_iterator_range(*this, 1, 0);
    }

    DEFINE(nary)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    }

    DEFINE(nary)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    /*---------.
    | weight.  |
    `---------*/

    DEFINE_CTOR(weight_node)(const weight_t& weight, value_t sub_exp)
      : sub_exp_(sub_exp)
      , weight_(weight)
    {}

    DEFINE(weight_node)::sub() const
      -> const value_t
    {
      return sub_exp_;
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

    DEFINE(weight_node)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(weight_node)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    }

    /*-----------.
    | constant.  |
    `-----------*/

    DEFINE(constant)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(constant)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };


# undef DEFINE_CTOR
# undef DEFINE

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_RATEXP_HXX_
