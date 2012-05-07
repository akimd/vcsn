#ifndef VCSN_CORE_RAT_NODE_HXX_
# define VCSN_CORE_RAT_NODE_HXX_

# include <vcsn/core/rat/node.hh>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    inline
    exp::~exp()
    {}


    /*-------.
    | node.  |
    `-------*/

    template <class Weight>
    inline
    node<Weight>::node(const weight_t& l)
      : lw_(l)
    {}

    template <class Weight>
    inline
    auto
    node<Weight>::left_weight() const
      -> const weight_t&
    {
      return lw_;
    }

    template <class Weight>
    inline
    auto
    node<Weight>::left_weight()
      -> weight_t&
    {
      return lw_;
    }


    /*--------.
    | inner.  |
    `--------*/

    template <class Weight>
    inline
    inner<Weight>::inner(const weight_t& l, const weight_t& r)
      : super_type(l)
      , rw_(r)
    {}

    template <class Weight>
    inline
    auto
    inner<Weight>::right_weight() const
      -> const weight_t&
    {
      return rw_;
    }

    template <class Weight>
    inline
    auto
    inner<Weight>::right_weight()
      -> weight_t&
    {
      return rw_;
    }


    /*-------.
    | leaf.  |
    `-------*/

    template <class Weight>
    inline
    leaf<Weight>::leaf(const weight_t& w)
      : super_type(w)
    {}


    /*-------.
    | nary.  |
    `-------*/

    template <class Weight>
    inline
    nary<Weight>::nary(const weight_t& l, const weight_t& r,
                       const nodes_t& ns)
      : super_type(l, r)
      , sub_node_(ns)
    {
    }


    template <class Weight>
    inline
    auto
    nary<Weight>::begin() const
      -> const_iterator
    {
      return sub_node_.begin();
    }

    template <class Weight>
    inline
    auto
    nary<Weight>::end() const
      -> const_iterator
    {
      return sub_node_.end();
    }

    template <class Weight>
    inline
    auto
    nary<Weight>::rbegin() const
      -> const_reverse_iterator
    {
      return sub_node_.begin();
    }

    template <class Weight>
    inline
    auto
    nary<Weight>::rend() const
      -> const_reverse_iterator
    {
      return sub_node_.end();
    }

    template <class Weight>
    inline
    size_t
    nary<Weight>::size() const
    {
      return sub_node_.size();
    }



    /*-------.
    | prod.  |
    `-------*/

    template <class Weight>
    inline
    prod<Weight>::prod(const weight_t& l, const weight_t& r,
                       const nodes_t& ns)
      : super_type(l, r, ns)
    {}


    template <class Weight>
    inline
    void
    prod<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }



    /*------.
    | sum.  |
    `------*/

    template <class Weight>
    inline
    sum<Weight>::sum(const weight_t& l, const weight_t& r,
                     const nodes_t& ns)
      : super_type(l, r, ns)
    {}

    template <class Weight>
    inline
    void
    sum<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }


    /*-------.
    | star.  |
    `-------*/

    template <class Weight>
    inline
    star<Weight>::star(const weight_t& l, const weight_t& r, kvalue_t sub_exp)
      : super_type(l, r)
      , sub_exp_(sub_exp)
    {}

    template <class Weight>
    inline
    auto
    star<Weight>::sub() const
      -> const kvalue_t
    {
      return sub_exp_;
    }


    template <class Weight>
    inline
    void
    star<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }


    /*------.
    | one.  |
    `------*/

    template <class Weight>
    inline
    one<Weight>::one(const weight_t& w)
      : super_type(w)
    {}

    template <class Weight>
    inline
    void
    one<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    /*-------.
    | zero.  |
    `-------*/

    template <class Weight>
    inline
    zero<Weight>::zero(const weight_t& w)
      : super_type(w)
    {}

    template <class Weight>
    inline
    void
    zero<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    /*-------.
    | atom.  |
    `-------*/

    template <class Weight>
    inline
    atom<Weight>::atom(const weight_t& w, const std::string& value)
      : super_type(w)
      , value_(value)
    {}

    template <class Weight>
    inline
    void
    atom<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    template <class Weight>
    inline
    const std::string&
    atom<Weight>::value() const
    {
      return value_;
    }

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_NODE_HXX_
