#ifndef VCSN_CORE_RAT_KRATEXP_HXX_
# define VCSN_CORE_RAT_KRATEXP_HXX_

# include <vcsn/core/rat/kratexp.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    inline
    exp::~exp()
    {}


#define DEFINE_CTOR(Node)                       \
    template <typename Atom, typename Weight>   \
    inline                                      \
    Node<Atom, Weight>::Node

#define DEFINE(Node)                            \
    template <typename Atom, typename Weight>   \
    inline                                      \
    auto                                        \
    Node<Atom, Weight>

    /*----------.
    | kratexp.  |
    `----------*/

    DEFINE_CTOR(kratexp)(const weight_t& l)
      : lw_(l)
    {}

    DEFINE(kratexp)::left_weight() const
      -> const weight_t&
    {
      return lw_;
    }

    DEFINE(kratexp)::left_weight()
      -> weight_t&
    {
      return lw_;
    }


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


    /*-------.
    | leaf.  |
    `-------*/

    DEFINE_CTOR(leaf)(const weight_t& w)
      : super_type(w)
    {}


    /*-------.
    | nary.  |
    `-------*/

    DEFINE_CTOR(nary)(const weight_t& l, const weight_t& r,
                       const kratexps_t& ns)
      : super_type(l, r)
      , sub_kratexp_(ns)
    {
    }


    DEFINE(nary)::begin() const
      -> const_iterator
    {
      return sub_kratexp_.begin();
    }

    DEFINE(nary)::end() const
      -> const_iterator
    {
      return sub_kratexp_.end();
    }

    DEFINE(nary)::rbegin() const
      -> const_reverse_iterator
    {
      return sub_kratexp_.begin();
    }

    DEFINE(nary)::rend() const
      -> const_reverse_iterator
    {
      return sub_kratexp_.end();
    }

    DEFINE(nary)::size() const
      -> size_t
    {
      return sub_kratexp_.size();
    }



    /*-------.
    | prod.  |
    `-------*/

    DEFINE_CTOR(prod)(const weight_t& l, const weight_t& r,
                       const kratexps_t& ns)
      : super_type(l, r, ns)
    {}


    DEFINE(prod)::accept(typename kratexp_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }



    /*------.
    | sum.  |
    `------*/

    DEFINE_CTOR(sum)(const weight_t& l, const weight_t& r,
                     const kratexps_t& ns)
      : super_type(l, r, ns)
    {}

    DEFINE(sum)::accept(typename kratexp_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }


    /*-------.
    | star.  |
    `-------*/

    DEFINE_CTOR(star)(const weight_t& l, const weight_t& r, kvalue_t sub_exp)
      : super_type(l, r)
      , sub_exp_(sub_exp)
    {}

    DEFINE(star)::sub() const
      -> const kvalue_t
    {
      return sub_exp_;
    }


    DEFINE(star)::accept(typename kratexp_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }


    /*------.
    | one.  |
    `------*/

    DEFINE_CTOR(one)(const weight_t& w)
      : super_type(w)
    {}

    DEFINE(one)::accept(typename kratexp_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    /*-------.
    | zero.  |
    `-------*/

    DEFINE_CTOR(zero)(const weight_t& w)
      : super_type(w)
    {}

    DEFINE(zero)::accept(typename kratexp_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    /*-------.
    | atom.  |
    `-------*/

    DEFINE_CTOR(atom)(const weight_t& w, const atom_value_t& value)
      : super_type(w)
      , value_(value)
    {}

    DEFINE(atom)::accept(typename kratexp_t::const_visitor& v) const
      -> void
    {
      v.visit(*this);
    }

    DEFINE(atom)::value() const
      -> const atom_value_t&
    {
      return value_;
    }

# undef DEFINE_CTOR
# undef DEFINE

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_KRATEXP_HXX_
