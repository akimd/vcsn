#include <cassert>

#include "visitor.hh"
#include "node.hh"

namespace vcsn {
  namespace rat_exp {

    visitor::visitor()
    { }

    visitor::~visitor()
    { }

    void
    visitor::visit(vcsn::rat_exp::exp& v)
    {
      assert(false);
    }

    void
    visitor::visit(vcsn::rat_exp::concat& v)
    {
      for(auto t : v)
        t->accept(*this);
    }

    void
    visitor::visit(vcsn::rat_exp::plus& v)
    {
      for(auto t : v)
        t->accept(*this);
    }

    void
    visitor::visit(vcsn::rat_exp::kleene& v)
    {
      exp *sub_exp = v.get_sub();
      if(sub_exp!= nullptr)
        sub_exp->accept(*this);
    }

    void
    visitor::visit(vcsn::rat_exp::one& v)
    { }

    void
    visitor::visit(vcsn::rat_exp::zero& v)
    { }

    void
    visitor::visit(vcsn::rat_exp::word& v)
    { }

    void
    visitor::visit(vcsn::rat_exp::left_weight& v)
    {
      exp *sub_node = v.get_exp();
      if(sub_node != nullptr)
        sub_node->accept(*this);
    }

    void
    visitor::visit(vcsn::rat_exp::right_weight& v)
    {
      exp *sub_node = v.get_exp();
      if(sub_node != nullptr)
        sub_node->accept(*this);
    }

  }// !rat_exp
}// !vcsn
