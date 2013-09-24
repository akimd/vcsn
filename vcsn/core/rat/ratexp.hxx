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

    DEFINE_CTOR(node)(const weight_t& l)
      : lw_(l)
    {}

    DEFINE(node)::left_weight() const
      -> const weight_t&
    {
      return lw_;
    }

    DEFINE(node)::left_weight()
      -> weight_t&
    {
      return lw_;
    }

    DEFINE(node)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };

    /*--------.
    | inner.  |
    `--------*/

    DEFINE_CTOR(inner)(const weight_t& l, const weight_t& r)
      : super_type(l)
      , rw_(r)
    {}

    DEFINE(inner)::right_weight() const
      -> const weight_t&
    {
      return rw_;
    }

    DEFINE(inner)::right_weight()
      -> weight_t&
    {
      return rw_;
    }

    DEFINE(inner)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };


    /*-------.
    | leaf.  |
    `-------*/

    DEFINE_CTOR(leaf)(const weight_t& w)
      : super_type(w)
    {}

    DEFINE(leaf)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };

    /*-------.
    | nary.  |
    `-------*/

    DEFINE_CTOR(nary)(const weight_t& l, const weight_t& r,
                       const ratexps_t& ns)
      : super_type(l, r)
      , sub_ratexp_(ns)
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
    };



    /*-------.
    | prod.  |
    `-------*/

    DEFINE_CTOR(prod)(const weight_t& l, const weight_t& r,
                       const ratexps_t& ns)
      : super_type(l, r, ns)
    {}


    DEFINE(prod)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(prod)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };



    /*------.
    | sum.  |
    `------*/

    DEFINE_CTOR(sum)(const weight_t& l, const weight_t& r,
                     const ratexps_t& ns)
      : super_type(l, r, ns)
    {}

    DEFINE(sum)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(sum)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };

    /*-------.
    | star.  |
    `-------*/

    DEFINE_CTOR(star)(const weight_t& l, const weight_t& r, value_t sub_exp)
      : super_type(l, r)
      , sub_exp_(sub_exp)
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


    /*------.
    | one.  |
    `------*/

    DEFINE_CTOR(one)(const weight_t& w)
      : super_type(w)
    {}

    DEFINE(one)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(one)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };


    /*-------.
    | zero.  |
    `-------*/

    DEFINE_CTOR(zero)(const weight_t& w)
      : super_type(w)
    {}

    DEFINE(zero)::accept(typename node_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(zero)::clone() const -> shared_t
    {
      return std::static_pointer_cast<const self_t>(clone_());
    };

    /*-------.
    | atom.  |
    `-------*/

    DEFINE_CTOR(atom)(const weight_t& w, const label_t& value)
      : super_type(w)
      , value_(value)
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

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_RATEXP_HXX_
