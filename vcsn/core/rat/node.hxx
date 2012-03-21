#ifndef VCSN_CORE_RAT_NODE_HXX_
# define VCSN_CORE_RAT_NODE_HXX_

# include <core/rat/node.hh>
# include <core/rat/visitor.hh>

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
    node<Weight>::~node()
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
    inner<Weight>::~inner()
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


    template <class Weight>
    inline
    leaf<Weight>::~leaf()
    {}


    /*-------.
    | nary.  |
    `-------*/

    template <class Weight>
    inline
    nary<Weight>::nary(const weight_t& l, const weight_t& r)
      : super_type(l, r)
    {
    }


    template <class Weight>
    inline
    nary<Weight>::~nary()
    {
      for (auto t : sub_node_)
        delete t;
    }

    template <class Weight>
    inline
    typename nary<Weight>::const_iterator
    nary<Weight>::begin() const
    {
      return sub_node_.begin();
    }

    template <class Weight>
    inline
    typename nary<Weight>::const_iterator
    nary<Weight>::end() const
    {
      return sub_node_.end();
    }

    template <class Weight>
    inline
    typename nary<Weight>::iterator
    nary<Weight>::begin()
    {
      return sub_node_.begin();
    }

    template <class Weight>
    inline
    typename nary<Weight>::iterator
    nary<Weight>::end()
    {
      return sub_node_.end();
    }

    template <class Weight>
    inline
    typename nary<Weight>::const_reverse_iterator
    nary<Weight>::rbegin() const
    {
      return sub_node_.begin();
    }

    template <class Weight>
    inline
    typename nary<Weight>::const_reverse_iterator
    nary<Weight>::rend() const
    {
      return sub_node_.end();
    }

    template <class Weight>
    inline
    typename nary<Weight>::reverse_iterator
    nary<Weight>::rbegin()
    {
      return sub_node_.begin();
    }

    template <class Weight>
    inline
    typename nary<Weight>::reverse_iterator
    nary<Weight>::rend()
    {
      return sub_node_.end();
    }

    template <class Weight>
    inline
    nary<Weight> &
    nary<Weight>::push_back(node_t* elt)
    {
      sub_node_.push_back(elt);
      return *this;
    }

    template <class Weight>
    inline
    nary<Weight> &
    nary<Weight>::push_front(node_t* elt)
    {
      sub_node_.push_front(elt);
      return *this;
    }

    template <class Weight>
    inline
    size_t
    nary<Weight>::size() const
    {
      return sub_node_.size();
    }

    template <class Weight>
    inline
    void
    nary<Weight>::erase(iterator it)
    {
      sub_node_.erase(it);
    }

    template <class Weight>
    inline
    void
    nary<Weight>::erase(iterator begin, iterator end)
    {
      sub_node_.erase(begin, end);
    }

    template <class Weight>
    inline
    void
    nary<Weight>::clear()
    {
      sub_node_.clear();
    }

    template <class Weight>
    inline
    void
    nary<Weight>::splice(iterator it, nary<Weight>& right)
    {
      assert(this->type() == right.type());
      sub_node_.splice(it, right.sub_node_);
    }


    /*-------.
    | prod.  |
    `-------*/

    template <class Weight>
    inline
    prod<Weight>::prod(const weight_t& l, const weight_t& r)
      : super_type(l, r)
    {}


    template <class Weight>
    inline
    void
    prod<Weight>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

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
    sum<Weight>::sum(const weight_t& l, const weight_t& r)
      : super_type(l, r)
    {}

    template <class Weight>
    inline
    void
    sum<Weight>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

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
    star<Weight>::star(const weight_t& l, const weight_t& r, node_t* sub_exp)
      : super_type(l, r)
      , sub_exp_(sub_exp)
    {}


    template <class Weight>
    inline
    star<Weight>::~star()
    {
      delete sub_exp_;
    }

    template <class Weight>
    inline
    auto
    star<Weight>::get_sub()
      -> node_t *
    {
      return sub_exp_;
    }

    template <class Weight>
    inline
    auto
    star<Weight>::get_sub() const
      -> const node_t *
    {
      return sub_exp_;
    }

    template <class Weight>
    inline
    void
    star<Weight>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
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
    one<Weight>::~one()
    {}

    template <class Weight>
    inline
    void
    one<Weight>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

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
    zero<Weight>::~zero()
    {}

    template <class Weight>
    inline
    void
    zero<Weight>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

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
    atom<Weight>::atom(const weight_t& w, std::string* atom)
      : super_type(w)
      , atom_(atom)
    {}

    template <class Weight>
    inline
    atom<Weight>::~atom()
    {
      delete atom_;
    }

    template <class Weight>
    inline
    void
    atom<Weight>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class Weight>
    inline
    void
    atom<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    template <class Weight>
    inline
    std::string*
    atom<Weight>::get_atom()
    {
      return atom_;
    }

    template <class Weight>
    inline
    const std::string*
    atom<Weight>::get_atom() const
    {
      return atom_;
    }

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_NODE_HXX_
