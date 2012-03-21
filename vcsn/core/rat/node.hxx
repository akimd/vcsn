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

    template <class WeightSet>
    inline
    node<WeightSet>::node()
    {}

    template <class WeightSet>
    inline
    node<WeightSet>::~node()
    {}



    /*-----------.
    | weighted.  |
    `-----------*/

    template <class WeightSet>
    inline
    weighted<WeightSet>::weighted()
      : ws_(st_ws_)
    {
      lw_ = ws_.unit();
      rw_ = ws_.unit();
    }

    template <class WeightSet>
    inline
    weighted<WeightSet>::weighted(const weightset_t& ws)
      : ws_(ws)
      , lw_(ws.unit())
      , rw_(ws.unit())
    {}

    template <class WeightSet>
    inline
    weighted<WeightSet>::~weighted()
    {}

    template <class WeightSet>
    inline
    const typename weighted<WeightSet>::weight_t&
    weighted<WeightSet>::left_weight() const
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    typename weighted<WeightSet>::weight_t&
    weighted<WeightSet>::left_weight()
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    const typename weighted<WeightSet>::weight_t&
    weighted<WeightSet>::right_weight() const
    {
      return rw_;
    }

    template <class WeightSet>
    inline
    typename weighted<WeightSet>::weight_t&
    weighted<WeightSet>::right_weight()
    {
      return rw_;
    }

    template <class WeightSet>
    inline
    const typename weighted<WeightSet>::weightset_t &
    weighted<WeightSet>::get_weight_set() const
    {
      return ws_;
    }

    template <class WeightSet>
    inline
    typename weighted<WeightSet>::WeightType
    weighted<WeightSet>::weight_type() const
    {
      return super_type::LR_WEIGHT;
    }

    /*----------------.
    | left_weighted.  |
    `----------------*/

    template <class WeightSet>
    inline
    left_weighted<WeightSet>::left_weighted()
      : ws_(st_ws_)
    {
      lw_ = ws_.unit();
    }

    template <class WeightSet>
    inline
    left_weighted<WeightSet>::left_weighted(const weightset_t& ws)
      : ws_(ws)
      , lw_(ws.unit())
    {}

    template <class WeightSet>
    inline
    left_weighted<WeightSet>::~left_weighted()
    {}

    template <class WeightSet>
    inline
    const typename left_weighted<WeightSet>::weight_t &
    left_weighted<WeightSet>::left_weight() const
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    typename left_weighted<WeightSet>::weight_t &
    left_weighted<WeightSet>::left_weight()
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    const typename left_weighted<WeightSet>::weightset_t &
    left_weighted<WeightSet>::get_weight_set() const
    {
      return ws_;
    }

    template <class WeightSet>
    inline
    typename left_weighted<WeightSet>::WeightType
    left_weighted<WeightSet>::weight_type() const
    {
      return super_type::L_WEIGHT;
    }

    /*-------.
    | nary.  |
    `-------*/

    template <class WeightSet>
    inline
    nary<WeightSet>::nary()
    {}

    template <class WeightSet>
    inline
    nary<WeightSet>::nary(weightset_t& ws)
      : weighted<WeightSet>(ws)
    {}

    template <class WeightSet>
    inline
    nary<WeightSet>::~nary()
    {
      for (auto t : sub_node_)
        delete t;
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::const_iterator
    nary<WeightSet>::begin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::const_iterator
    nary<WeightSet>::end() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::iterator
    nary<WeightSet>::begin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::iterator
    nary<WeightSet>::end()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::const_reverse_iterator
    nary<WeightSet>::rbegin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::const_reverse_iterator
    nary<WeightSet>::rend() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::reverse_iterator
    nary<WeightSet>::rbegin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename nary<WeightSet>::reverse_iterator
    nary<WeightSet>::rend()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    nary<WeightSet> &
    nary<WeightSet>::push_back(node_t* elt)
    {
      sub_node_.push_back(elt);
      return *this;
    }

    template <class WeightSet>
    inline
    nary<WeightSet> &
    nary<WeightSet>::push_front(node_t* elt)
    {
      sub_node_.push_front(elt);
      return *this;
    }

    template <class WeightSet>
    inline
    size_t
    nary<WeightSet>::size() const
    {
      return sub_node_.size();
    }

    template <class WeightSet>
    inline
    void
    nary<WeightSet>::erase(iterator it)
    {
      sub_node_.erase(it);
    }

    template <class WeightSet>
    inline
    void
    nary<WeightSet>::erase(iterator begin, iterator end)
    {
      sub_node_.erase(begin, end);
    }

    template <class WeightSet>
    inline
    void
    nary<WeightSet>::clear()
    {
      sub_node_.clear();
    }

    template <class WeightSet>
    inline
    void
    nary<WeightSet>::splice(iterator it, nary<WeightSet>& right)
    {
      assert(this->type() == right.type());
      sub_node_.splice(it, right.sub_node_);
    }

    /*-------.
    | prod.  |
    `-------*/

    template <class WeightSet>
    inline
    prod<WeightSet>::prod(weightset_t& ws)
      : super_type(ws)
    {}


    template <class WeightSet>
    inline
    void
    prod<WeightSet>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    prod<WeightSet>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }



    /*------.
    | sum.  |
    `------*/

    template <class WeightSet>
    inline
    sum<WeightSet>::sum(weightset_t& ws)
      : super_type(ws)
    {}

    template <class WeightSet>
    inline
    void
    sum<WeightSet>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    sum<WeightSet>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }


    /*-------.
    | star.  |
    `-------*/

    template <class WeightSet>
    inline
    star<WeightSet>::star(node_t* sub_exp)
      : weighted<WeightSet>()
      , sub_exp_(sub_exp)
    {}

    template <class WeightSet>
    inline
    star<WeightSet>::star(node_t* sub_exp, const weightset_t& ws)
      : weighted<WeightSet>(node_t::KLEENE, ws)
      , sub_exp_(sub_exp)
    {}

    template <class WeightSet>
    inline
    star<WeightSet>::~star()
    {
      delete sub_exp_;
    }

    template <class WeightSet>
    inline
    auto
    star<WeightSet>::get_sub()
      -> node_t *
    {
      return sub_exp_;
    }

    template <class WeightSet>
    inline
    auto
    star<WeightSet>::get_sub() const
      -> const node_t *
    {
      return sub_exp_;
    }

    template <class WeightSet>
    inline
    void
    star<WeightSet>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    star<WeightSet>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    /*------.
    | one.  |
    `------*/

    template <class WeightSet>
    inline
    one<WeightSet>::one()
    {}

    template <class WeightSet>
    inline
    one<WeightSet>::~one()
    {}

    template <class WeightSet>
    inline
    void
    one<WeightSet>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    one<WeightSet>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    /*-------.
    | zero.  |
    `-------*/

    template <class WeightSet>
    inline
    zero<WeightSet>::zero()
    {}

    template <class WeightSet>
    inline
    zero<WeightSet>::~zero()
    {}

    template <class WeightSet>
    inline
    void
    zero<WeightSet>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    zero<WeightSet>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    /*-------.
    | word.  |
    `-------*/

    template <class WeightSet>
    inline
    word<WeightSet>::word(std::string* word)
      : left_weighted<WeightSet>()
      , word_(word)
    {}

    template <class WeightSet>
    inline
    word<WeightSet>::word(std::string* word, const weightset_t& ws)
      : left_weighted<WeightSet>(ws)
      , word_(word)
    {}

    template <class WeightSet>
    inline
    word<WeightSet>::~word()
    {
      delete word_;
    }

    template <class WeightSet>
    inline
    void
    word<WeightSet>::accept(typename node_t::visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    word<WeightSet>::accept(typename node_t::const_visitor& v) const
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    std::string*
    word<WeightSet>::get_word()
    {
      return word_;
    }

    template <class WeightSet>
    inline
    const std::string*
    word<WeightSet>::get_word() const
    {
      return word_;
    }

  } // namespace exp
} // namespace vcsn

#endif // !VCSN_CORE_RAT_NODE_HXX_
