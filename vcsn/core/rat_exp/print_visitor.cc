#include <cassert>

#include "print_visitor.hh"

namespace vcsn {
  namespace rat_exp {

    PrintVisitor::PrintVisitor(std::ostream &out) :
      out_(out)
    { }

    PrintVisitor::~PrintVisitor()
    {
      out << std::flush;
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
        out << '(';

      concat::const_iterator end = v.end();
      concat::cons_iterator it   = v.begin();
      it->accept(*this);
      ++it;
      for(; end != it; ++it)
      {
        out << '*';
        it->accept(*this);
        out << ')';
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::plus& v)
    {
      for(unsigned i = v.size(); i != 1; --i)
        out << '(';

      concat::const_iterator end = v.end();
      concat::cons_iterator it   = v.begin();
      it->accept(*this);
      ++it;
      for(; end != it; ++it)
      {
        out << '+';
        it->accept(*this);
        out << ')';
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::kleene& v)
    {
      exp *sub_exp = v.get_sub();
      if(sub_exp!= nullptr)
      {
        out << '(';
        sub_exp->acept(*this);
        out << ")*";
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::one& v)
    {
      out << "\\e";
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::zero& v)
    {
      out << "\\z";
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::word& v)
    {
      assert(false);
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::left_weight& v)
    {
      exp *sub_node = v.get_exp();
      if(sub_node != nullptr)
      {
        out << "({"
            << *v.get_weight() // FIXME: weight could not be printed
                               // like that.
            << '}';

        sub_node->accept(*this);
        out << ')';
      }
    }

    void
    PrintVisitor::visit(vcsn::rat_exp::right_weight& v)
    {
      exp *sub_node = v.get_exp();
      if(sub_node != nullptr)
      {
        out << '(';
        sub_node->accept(*this);
        out << '{'
            << *v.get_weight() // FIXME: weight could not be printed
                               // like that
            << "})";
      }
    }

  } // rat_exp
} // vcsn
