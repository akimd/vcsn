#ifndef VCSN_CORE_RAT_EXP_NODE_HXX_
# define VCSN_CORE_RAT_EXP_NODE_HXX_

# include "node.hh"
# include "visitor.hh"

namespace vcsn {
  namespace rat_exp {

    ////////////
    // Concat //
    ////////////

    inline
    concat::concat()
    { }

    inline
    concat::~concat()
    {
      for(auto t : sub_node_)
        delete t;
    }

    inline
    concat &
    concat::push_front(exp *l_exp)
    {
      sub_node_.push_front(l_exp);
    }

    inline
    concat &
    concat::push_back(exp *l_exp)
    {
      sub_node_.push_back(l_exp);
    }

    inline
    void
    concat::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    inline
    concat::const_iterator
    concat::begin() const
    {
      return sub_node_.begin();
    }

    inline
    concat::iterator
    concat::begin()
    {
      return sub_node_.begin();
    }

    inline
    concat::const_iterator
    concat::end() const
    {
      return sub_node_.end();
    }

    inline
    concat::iterator
    concat::end()
    {
      return sub_node_.end();
    }

    inline
    concat::const_reverse_iterator
    concat::rbegin() const
    {
      return sub_node_.rbegin();
    }

    inline
    concat::reverse_iterator
    concat::rbegin()
    {
      return sub_node_.rbegin();
    }

    inline
    concat::const_reverse_iterator
    concat::rend() const
    {
      return sub_node_.rend();
    }

    inline
    concat::reverse_iterator
    concat::rend()
    {
      return sub_node_.rend();
    }

    //////////
    // Plus //
    //////////

    inline
    plus::plus()
    { }

    inline
    plus::~plus()
    {
      for(auto t : sub_node_)
        delete t;
    }

    inline
    plus &
    plus::push_front(exp *l_exp)
    {
      sub_node_.push_front(l_exp);
    }

    inline
    plus &
    plus::push_back(exp *l_exp)
    {
      sub_node_.push_back(l_exp);
    }

    inline
    void
    plus::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    inline
    plus::const_iterator
    plus::begin() const
    {
      return sub_node_.begin();
    }

    inline
    plus::iterator plus::begin()
    {
      return sub_node_.begin();
    }

    inline
    plus::const_iterator
    plus::end() const
    {
      return sub_node_.end();
    }

    inline
    plus::iterator
    plus::end()
    {
      return sub_node_.end();
    }

    inline
    plus::const_reverse_iterator
    plus::rbegin() const
    {
      return sub_node_.rbegin();
    }

    inline
    plus::reverse_iterator
    plus::rbegin()
    {
      return sub_node_.rbegin();
    }

    inline
    plus::const_reverse_iterator
    plus::rend() const
    {
      return sub_node_.rend();
    }

    inline
    plus::reverse_iterator
    plus::rend()
    {
      return sub_node_.rend();
    }

    ////////////
    // Kleene //
    ////////////

    inline
    kleene::kleene(exp *sub_exp) :
      sub_exp_(sub_exp)
    { }

    inline
    kleene::~kleene()
    {
      delete sub_exp_;
    }

    inline
    void
    kleene::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    inline
    exp *
    kleene::get_sub()
    {
      return sub_exp_;
    }

    /////////////
    // One cst //
    /////////////

    inline
    one::one()
    { }

    inline
    one::~one()
    { }

    inline
    void
    one::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    //////////////
    // Zero cst //
    //////////////

    inline
    zero::zero()
    { }

    inline
    zero::~zero()
    { }

    inline
    void
    zero::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    //////////
    // Word //
    //////////

    inline
    word::word(std::string *word) :
      word_(word)
    { }

    inline
    word::~word()
    {
      delete word_;
    }

    inline
    void
    word::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    /////////////////
    // Left Weight //
    /////////////////

    inline
    left_weight::left_weight(left_weight::weight *l_weight, exp *r_exp) :
      l_weight_(l_weight),
      r_exp_(r_exp)
    { }

    inline
    left_weight::~left_weight()
    {
      delete r_exp_;
      delete l_weight_;
    }

    inline
    void
    left_weight::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    inline
    exp *
    left_weight::get_exp()
    {
      return r_exp_;
    }

    inline
    left_weight::weight *
    left_weight::get_weight()
    {
      return l_weight_;
    }

    //////////////////
    // Right Weight //
    //////////////////

    inline
    right_weight::right_weight(exp *l_exp, right_weight::weight *r_weight) :
      l_exp_(l_exp),
      r_weight_(r_weight)
    { }

    inline
    right_weight::~right_weight()
    {
      delete r_weight_;
      delete l_exp_;
    }

    inline
    void
    right_weight::accept(vcsn::rat_exp::visitor &v)
    {
      v.visit(*this);
    }

    inline
    exp *
    right_weight::get_exp()
    {
      return l_exp_;
    }

    inline
    right_weight::weight *
    right_weight::get_weight()
    {
      return r_weight_;
    }

  } // !exp
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_NODE_HXX_
