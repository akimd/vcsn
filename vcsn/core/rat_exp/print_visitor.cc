#include <cassert>

#include <core/rat_exp/print_visitor.hh>
#include <core/rat_exp/node.hh>

namespace vcsn {
  namespace rat_exp {

    PrintVisitor::PrintVisitor(std::ostream &out) :
      out_(out)
    { }

    PrintVisitor::~PrintVisitor()
    {
      out_ << std::endl;
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::exp& v)
    {
      assert(false);
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::concat& v)
    {
      for(unsigned i = v.size(); i != 1; --i)
        out_ << '(';

      concat::const_iterator end = v.end();
      concat::const_iterator it  = v.begin();
      (* it)->accept(*this);
      ++it;
      for(; end != it; ++it)
      {
        out_ << '.';
        (* it)->accept(*this);
        out_ << ')';
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::plus& v)
    {
      for(unsigned i = v.size(); i != 1; --i)
        out_ << '(';

      concat::const_iterator end = v.end();
      concat::const_iterator it  = v.begin();
      (* it)->accept(*this);
      ++it;
      for(; end != it; ++it)
      {
        out_ << '+';
        (* it)->accept(*this);
        out_ << ')';
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::kleene& v)
    {
      exp *sub_exp = v.get_sub();
      if(sub_exp != nullptr)
      {
        out_ << '(';
        sub_exp->accept(*this);
        out_ << ")*";
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::one& v)
    {
      out_ << "\\e";
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::zero& v)
    {
      out_ << "\\z";
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::word& v)
    {
      out_ << *v.get_word();
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::left_weight& v)
    {
      exp *sub_node = v.get_exp();
      if(sub_node != nullptr)
      {
        out_ << '(';
        for(weight_type *s : (*v.get_weight()))
        {
          out_ << '{'
               << *s
               << '}';
        }
        sub_node->accept(*this);
        out_ << ')';
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::right_weight& v)
    {
      exp *sub_node = v.get_exp();
      if(sub_node != nullptr)
      {
        out_ << '(';
        sub_node->accept(*this);
        for(weight_type *s : (*v.get_weight()))
        {
          out_ << '{'
               << *s
               << '}';
        }
        out_ << ')';
      }
    }

  } // rat_exp
} // vcsn
