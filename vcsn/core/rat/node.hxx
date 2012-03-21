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
    node<Weight>::node()
    {}

    template <class Weight>
    inline
    node<Weight>::~node()
    {}



    /*-----------.
    | weighted.  |
    `-----------*/

    template <class Weight>
    inline
    weighted<Weight>::weighted(const weight_t& l, const weight_t& r)
      : lw_(l)
      , rw_(r)
    {}


    template <class Weight>
    inline
    weighted<Weight>::~weighted()
    {}

    template <class Weight>
    inline
    auto
    weighted<Weight>::left_weight() const
      -> const weight_t&
    {
      return lw_;
    }

    template <class Weight>
    inline
    auto
    weighted<Weight>::left_weight()
      -> weight_t&
    {
      return lw_;
    }

    template <class Weight>
    inline
    auto
    weighted<Weight>::right_weight() const
      -> const weight_t&
    {
      return rw_;
    }

    template <class Weight>
    inline
    auto
    weighted<Weight>::right_weight()
      -> weight_t&
    {
      return rw_;
    }

    template <class Weight>
    inline
    auto
    weighted<Weight>::weight_type() const
      -> WeightType
    {
      return super_type::LR_WEIGHT;
    }

    /*----------------.
    | left_weighted.  |
    `----------------*/

    template <class Weight>
    inline
    left_weighted<Weight>::left_weighted(const weight_t& w)
      : lw_(w)
    {}


    template <class Weight>
    inline
    left_weighted<Weight>::~left_weighted()
    {}

    template <class Weight>
    inline
    auto
    left_weighted<Weight>::left_weight() const
      -> const weight_t &
    {
      return lw_;
    }

    template <class Weight>
    inline
    auto
    left_weighted<Weight>::left_weight()
      -> weight_t &
    {
      return lw_;
    }


    template <class Weight>
    inline
    auto
    left_weighted<Weight>::weight_type() const
      -> WeightType
    {
      return super_type::L_WEIGHT;
    }


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
    | word.  |
    `-------*/

    template <class Weight>
    inline
    word<Weight>::word(const weight_t& w, std::string* word)
      : super_type(w)
      , word_(word)
    {}

    template <class Weight>
    inline
    word<Weight>::~word()
    {
      delete word_;
    }

    template <class Weight>
    inline
    void
    word<Weight>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class Weight>
    inline
    void
    word<Weight>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    template <class Weight>
    inline
    std::string*
    word<Weight>::get_word()
    {
      return word_;
    }

    template <class Weight>
    inline
    const std::string*
    word<Weight>::get_word() const
    {
      return word_;
    }

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_NODE_HXX_
