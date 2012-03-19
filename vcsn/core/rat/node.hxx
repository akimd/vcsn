#ifndef VCSN_CORE_RAT_NODE_HXX_
# define VCSN_CORE_RAT_NODE_HXX_

# include <core/rat/node.hh>
# include <core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    inline
    RatExp::~RatExp()
    {}

    ////////////////
    // node //
    ////////////////

    template <class WeightSet>
    inline
    node<WeightSet>::node()
    {}

    template <class WeightSet>
    inline
    node<WeightSet>::~node()
    {}

    //////////////////
    // LRWeightNode //
    //////////////////

    template <class WeightSet>
    inline
    LRWeightNode<WeightSet>::LRWeightNode()
      : ws_(st_ws_)
    {
      lw_ = ws_.unit();
      rw_ = ws_.unit();
    }

    template <class WeightSet>
    inline
    LRWeightNode<WeightSet>::LRWeightNode(const weightset_t& ws)
      : ws_(ws)
      , lw_(ws.unit())
      , rw_(ws.unit())
    {}

    template <class WeightSet>
    inline
    LRWeightNode<WeightSet>::~LRWeightNode()
    {}

    template <class WeightSet>
    inline
    const typename LRWeightNode<WeightSet>::weight_t &
    LRWeightNode<WeightSet>::left_weight() const
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    typename LRWeightNode<WeightSet>::weight_t &
    LRWeightNode<WeightSet>::left_weight()
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    const typename LRWeightNode<WeightSet>::weight_t &
    LRWeightNode<WeightSet>::right_weight() const
    {
      return rw_;
    }

    template <class WeightSet>
    inline
    typename LRWeightNode<WeightSet>::weight_t &
    LRWeightNode<WeightSet>::right_weight()
    {
      return rw_;
    }

    template <class WeightSet>
    inline
    const typename LRWeightNode<WeightSet>::weightset_t &
    LRWeightNode<WeightSet>::get_weight_set() const
    {
      return ws_;
    }

    template <class WeightSet>
    inline
    typename LRWeightNode<WeightSet>::WeightType
    LRWeightNode<WeightSet>::weight_type() const
    {
      return super_type::LR_WEIGHT;
    }

    /////////////////
    // LWeightNode //
    /////////////////

    template <class WeightSet>
    inline
    LWeightNode<WeightSet>::LWeightNode()
      : ws_(st_ws_)
    {
      lw_ = ws_.unit();
    }

    template <class WeightSet>
    inline
    LWeightNode<WeightSet>::LWeightNode(const weightset_t& ws)
      : ws_(ws)
      , lw_(ws.unit())
    {}

    template <class WeightSet>
    inline
    LWeightNode<WeightSet>::~LWeightNode()
    {}

    template <class WeightSet>
    inline
    const typename LWeightNode<WeightSet>::weight_t &
    LWeightNode<WeightSet>::left_weight() const
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    typename LWeightNode<WeightSet>::weight_t &
    LWeightNode<WeightSet>::left_weight()
    {
      return lw_;
    }

    template <class WeightSet>
    inline
    const typename LWeightNode<WeightSet>::weightset_t &
    LWeightNode<WeightSet>::get_weight_set() const
    {
      return ws_;
    }

    template <class WeightSet>
    inline
    typename LWeightNode<WeightSet>::WeightType
    LWeightNode<WeightSet>::weight_type() const
    {
      return super_type::L_WEIGHT;
    }

    //////////////////
    // concat //
    //////////////////

    template <class WeightSet>
    inline
    concat<WeightSet>::concat()
    {}

    template <class WeightSet>
    inline
    concat<WeightSet>::concat(weightset_t& ws)
      : LRWeightNode<WeightSet>(ws)
    {}

    template <class WeightSet>
    inline
    concat<WeightSet>::~concat()
    {
      for (auto t : sub_node_)
        delete t;
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::const_iterator
    concat<WeightSet>::begin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::const_iterator
    concat<WeightSet>::end() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::iterator
    concat<WeightSet>::begin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::iterator
    concat<WeightSet>::end()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::const_reverse_iterator
    concat<WeightSet>::rbegin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::const_reverse_iterator
    concat<WeightSet>::rend() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::reverse_iterator
    concat<WeightSet>::rbegin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename concat<WeightSet>::reverse_iterator
    concat<WeightSet>::rend()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    concat<WeightSet> &
    concat<WeightSet>::push_back(node_t* elt)
    {
      sub_node_.push_back(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    concat<WeightSet> &
    concat<WeightSet>::push_front(node_t* elt)
    {
      sub_node_.push_front(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    size_t
    concat<WeightSet>::size() const
    {
      return sub_node_.size();
    }

    template <class WeightSet>
    inline
    void
    concat<WeightSet>::erase(iterator it)
    {
      sub_node_.erase(it);
    }

    template <class WeightSet>
    inline
    void
    concat<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    concat<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    //////////////////
    // plus //
    //////////////////

    template <class WeightSet>
    inline
    plus<WeightSet>::plus()
    {}

    template <class WeightSet>
    inline
    plus<WeightSet>::plus(const weightset_t& ws)
      : LRWeightNode<WeightSet>(ws)
    {}

    template <class WeightSet>
    inline
    plus<WeightSet>::~plus()
    {
      for (auto t : sub_node_)
        delete t;
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::const_iterator
    plus<WeightSet>::begin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::const_iterator
    plus<WeightSet>::end() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::iterator
    plus<WeightSet>::begin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::iterator
    plus<WeightSet>::end()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::const_reverse_iterator
    plus<WeightSet>::rbegin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::const_reverse_iterator
    plus<WeightSet>::rend() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::reverse_iterator
    plus<WeightSet>::rbegin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename plus<WeightSet>::reverse_iterator
    plus<WeightSet>::rend()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    plus<WeightSet> &
    plus<WeightSet>::push_back(node_t* elt)
    {
      sub_node_.push_back(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    plus<WeightSet> &
    plus<WeightSet>::push_front(node_t* elt)
    {
      sub_node_.push_front(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    size_t
    plus<WeightSet>::size() const
    {
      return sub_node_.size();
    }

    template <class WeightSet>
    inline
    void
    plus<WeightSet>::erase(iterator it)
    {
      sub_node_.erase(it);
    }

    template <class WeightSet>
    inline
    void
    plus<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    plus<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    //////////////////
    // kleene //
    //////////////////

    template <class WeightSet>
    inline
    kleene<WeightSet>::kleene(node_t* sub_exp)
      : LRWeightNode<WeightSet>()
      , sub_exp_(sub_exp)
    {}

    template <class WeightSet>
    inline
    kleene<WeightSet>::kleene(node_t* sub_exp, const weightset_t& ws)
      : LRWeightNode<WeightSet>(node_t::KLEENE, ws)
      , sub_exp_(sub_exp)
    {}

    template <class WeightSet>
    inline
    kleene<WeightSet>::~kleene()
    {
      delete sub_exp_;
    }

    template <class WeightSet>
    inline
    auto
    kleene<WeightSet>::get_sub()
      -> node_t *
    {
      return sub_exp_;
    }

    template <class WeightSet>
    inline
    auto
    kleene<WeightSet>::get_sub() const
      -> const node_t *
    {
      return sub_exp_;
    }

    template <class WeightSet>
    inline
    void
    kleene<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    kleene<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    ///////////////
    // one //
    ///////////////

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
    one<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    one<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    ////////////////
    // zero //
    ////////////////

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
    zero<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    zero<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    ////////////////
    // word //
    ////////////////

    template <class WeightSet>
    inline
    word<WeightSet>::word(std::string* word)
      : LWeightNode<WeightSet>()
      , word_(word)
    {}

    template <class WeightSet>
    inline
    word<WeightSet>::word(std::string* word, const weightset_t& ws)
      : LWeightNode<WeightSet>(ws)
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
    word<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    word<WeightSet>::accept(typename node_t::ConstVisitor& v) const
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

  } // exp
} // vcsn

#endif // !VCSN_CORE_RAT_NODE_HXX_
