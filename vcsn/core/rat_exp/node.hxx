#ifndef VCSN_CORE_RAT_EXP_NODE_HXX_
# define VCSN_CORE_RAT_EXP_NODE_HXX_

# include <core/rat_exp/node.hh>
# include <core/rat_exp/visitor.hh>

namespace vcsn
{
  namespace rat_exp
  {

    inline
    RatExp::~RatExp()
    {}

    ////////////////
    // RatExpNode //
    ////////////////

    template <class WeightSet>
    inline
    RatExpNode<WeightSet>::RatExpNode()
    {}

    template <class WeightSet>
    inline
    RatExpNode<WeightSet>::~RatExpNode()
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
    // RatExpConcat //
    //////////////////

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>::RatExpConcat()
    {}

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>::RatExpConcat(weightset_t& ws)
      : LRWeightNode<WeightSet>(ws)
    {}

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet>::~RatExpConcat()
    {
      for (auto t : sub_node_)
        delete t;
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::const_iterator
    RatExpConcat<WeightSet>::begin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::const_iterator
    RatExpConcat<WeightSet>::end() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::iterator
    RatExpConcat<WeightSet>::begin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::iterator
    RatExpConcat<WeightSet>::end()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::const_reverse_iterator
    RatExpConcat<WeightSet>::rbegin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::const_reverse_iterator
    RatExpConcat<WeightSet>::rend() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::reverse_iterator
    RatExpConcat<WeightSet>::rbegin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpConcat<WeightSet>::reverse_iterator
    RatExpConcat<WeightSet>::rend()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet> &
    RatExpConcat<WeightSet>::push_back(node_t* elt)
    {
      sub_node_.push_back(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    RatExpConcat<WeightSet> &
    RatExpConcat<WeightSet>::push_front(node_t* elt)
    {
      sub_node_.push_front(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    size_t
    RatExpConcat<WeightSet>::size() const
    {
      return sub_node_.size();
    }

    template <class WeightSet>
    inline
    void
    RatExpConcat<WeightSet>::erase(iterator it)
    {
      sub_node_.erase(it);
    }

    template <class WeightSet>
    inline
    void
    RatExpConcat<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    RatExpConcat<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    //////////////////
    // RatExpPlus //
    //////////////////

    template <class WeightSet>
    inline
    RatExpPlus<WeightSet>::RatExpPlus()
    {}

    template <class WeightSet>
    inline
    RatExpPlus<WeightSet>::RatExpPlus(const weightset_t& ws)
      : LRWeightNode<WeightSet>(ws)
    {}

    template <class WeightSet>
    inline
    RatExpPlus<WeightSet>::~RatExpPlus()
    {
      for (auto t : sub_node_)
        delete t;
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::const_iterator
    RatExpPlus<WeightSet>::begin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::const_iterator
    RatExpPlus<WeightSet>::end() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::iterator
    RatExpPlus<WeightSet>::begin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::iterator
    RatExpPlus<WeightSet>::end()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::const_reverse_iterator
    RatExpPlus<WeightSet>::rbegin() const
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::const_reverse_iterator
    RatExpPlus<WeightSet>::rend() const
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::reverse_iterator
    RatExpPlus<WeightSet>::rbegin()
    {
      return sub_node_.begin();
    }

    template <class WeightSet>
    inline
    typename RatExpPlus<WeightSet>::reverse_iterator
    RatExpPlus<WeightSet>::rend()
    {
      return sub_node_.end();
    }

    template <class WeightSet>
    inline
    RatExpPlus<WeightSet> &
    RatExpPlus<WeightSet>::push_back(node_t* elt)
    {
      sub_node_.push_back(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    RatExpPlus<WeightSet> &
    RatExpPlus<WeightSet>::push_front(node_t* elt)
    {
      sub_node_.push_front(elt);
      return* this;
    }

    template <class WeightSet>
    inline
    size_t
    RatExpPlus<WeightSet>::size() const
    {
      return sub_node_.size();
    }

    template <class WeightSet>
    inline
    void
    RatExpPlus<WeightSet>::erase(iterator it)
    {
      sub_node_.erase(it);
    }

    template <class WeightSet>
    inline
    void
    RatExpPlus<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    RatExpPlus<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    //////////////////
    // RatExpKleene //
    //////////////////

    template <class WeightSet>
    inline
    RatExpKleene<WeightSet>::RatExpKleene(node_t* sub_exp)
      : LRWeightNode<WeightSet>()
      , sub_exp_(sub_exp)
    {}

    template <class WeightSet>
    inline
    RatExpKleene<WeightSet>::RatExpKleene(node_t* sub_exp, const weightset_t& ws)
      : LRWeightNode<WeightSet>(node_t::KLEENE, ws)
      , sub_exp_(sub_exp)
    {}

    template <class WeightSet>
    inline
    RatExpKleene<WeightSet>::~RatExpKleene()
    {
      delete sub_exp_;
    }

    template <class WeightSet>
    inline
    auto
    RatExpKleene<WeightSet>::get_sub()
      -> node_t *
    {
      return sub_exp_;
    }

    template <class WeightSet>
    inline
    auto
    RatExpKleene<WeightSet>::get_sub() const
      -> const node_t *
    {
      return sub_exp_;
    }

    template <class WeightSet>
    inline
    void
    RatExpKleene<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    RatExpKleene<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    ///////////////
    // RatExpOne //
    ///////////////

    template <class WeightSet>
    inline
    RatExpOne<WeightSet>::RatExpOne()
    {}

    template <class WeightSet>
    inline
    RatExpOne<WeightSet>::~RatExpOne()
    {}

    template <class WeightSet>
    inline
    void
    RatExpOne<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    RatExpOne<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    ////////////////
    // RatExpZero //
    ////////////////

    template <class WeightSet>
    inline
    RatExpZero<WeightSet>::RatExpZero()
    {}

    template <class WeightSet>
    inline
    RatExpZero<WeightSet>::~RatExpZero()
    {}

    template <class WeightSet>
    inline
    void
    RatExpZero<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    RatExpZero<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    ////////////////
    // RatExpWord //
    ////////////////

    template <class WeightSet>
    inline
    RatExpWord<WeightSet>::RatExpWord(std::string* word)
      : LWeightNode<WeightSet>()
      , word_(word)
    {}

    template <class WeightSet>
    inline
    RatExpWord<WeightSet>::RatExpWord(std::string* word, const weightset_t& ws)
      : LWeightNode<WeightSet>(ws)
      , word_(word)
    {}

    template <class WeightSet>
    inline
    RatExpWord<WeightSet>::~RatExpWord()
    {
      delete word_;
    }

    template <class WeightSet>
    inline
    void
    RatExpWord<WeightSet>::accept(typename node_t::Visitor& v)
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    void
    RatExpWord<WeightSet>::accept(typename node_t::ConstVisitor& v) const
    {
      v.visit(*this);
    }

    template <class WeightSet>
    inline
    std::string*
    RatExpWord<WeightSet>::get_word()
    {
      return word_;
    }

    template <class WeightSet>
    inline
    const std::string*
    RatExpWord<WeightSet>::get_word() const
    {
      return word_;
    }

  } // exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_NODE_HXX_
