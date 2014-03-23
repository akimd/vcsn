#include <vcsn/dyn/context-printer.hh>
#include <vcsn/dyn/type-ast.hh>

#include <vcsn/misc/indent.hh>


namespace vcsn
{
  namespace ast
  {

    void context_printer::header(std::string h)
    {
      // Open code some mismatches between algo name, and header
      // name.  FIXME: algorithms should register this temselves.
      if (false) {}
#define ALGO(In, Out)                       \
      else if (h == "vcsn/algos/" In ".hh") \
        h = "vcsn/algos/" Out ".hh"
      ALGO("chain_ratexp", "concatenation");
      ALGO("concatenate_ratexp", "concatenate");
      ALGO("copy_ratexp", "copy");
      ALGO("infiltration", "product");
      ALGO("info_ratexp", "info");
      ALGO("intersection_ratexp", "product");
      ALGO("left_mult_ratexp", "left_mult");
      ALGO("make_context", "make-context");
      ALGO("print_ratexp", "print");
      ALGO("print_weight", "print");
      ALGO("right_mult", "left_mult");
      ALGO("right_mult_ratexp", "left_mult");
      ALGO("shuffle", "product");
      ALGO("sum_ratexp", "sum");
      ALGO("union_a", "union");
      ALGO("union_ratexp", "union");
#undef ALGO
      headers_.insert(h);
    }

    std::ostream& context_printer::print(std::ostream& o)
    {
      //o << "// " << is.str() << "\n";
      o <<
        "#define BUILD_LIBVCSN 1\n"
        "#define VCSN_INSTANTIATION 1\n"
        "#define MAYBE_EXTERN\n"
        "\n";
      for (const auto& h: headers_)
        o << "#include <" << h << ">\n";
      o << '\n';
      for (const auto& h: headers_late_)
        o << "#include <" << h << ">\n";
      o << "\n"
        << os_.str();
      return o;
    }

#define DEFINE(Type)              \
    void context_printer::visit(const Type& t)

    DEFINE(automaton)
    {
      os_ << "vcsn::mutable_automaton<" << incendl;
      t.get_content()->accept(*this);
      os_ << decendl << '>';
      header("vcsn/core/mutable_automaton.hh");
    }

    DEFINE(context)
    {
      header("vcsn/ctx/context.hh");
      os_ << "vcsn::ctx::context<" << incendl;
      t.get_labelset()->accept(*this);
      os_ << ',' << iendl;
      t.get_weightset()->accept(*this);
      os_ << decendl << '>';
    }

    DEFINE(tupleset)
    {
      headers_late_.insert("vcsn/labelset/tupleset.hh");
      os_ << "vcsn::tupleset<" << incendl;
      auto v = t.get_sets();
      for (unsigned int i = 0; i < v.size() - 1; ++i)
      {
        v[i]->accept(*this);
        os_ << ',' << iendl;
      }
      v[v.size() - 1]->accept(*this);
      os_ << decendl << '>';
    }


    DEFINE(nullableset)
    {
      header("vcsn/ctx/lan_char.hh");
      os_ << "vcsn::nullableset<" << incendl;
      t.get_labelset()->accept(*this);
      os_ << decendl << ">";
    }

    DEFINE(oneset)
    {
      (void) t;
      header("vcsn/labelset/oneset.hh");
      os_ << "vcsn::ctx::oneset";
    }

    DEFINE(letterset)
    {
      (void) t;
      header("vcsn/labelset/letterset.hh");
      // Some instantiation happen here:
      header("vcsn/ctx/lal_char.hh");
      os_ << "vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters>>";
    }

    DEFINE(ratexpset)
    {
      os_ << "vcsn::ratexpset<" << incendl;
      t.get_context()->accept(*this);
      os_ << decendl << '>';
      header("vcsn/core/rat/ratexpset.hh");
    }

    DEFINE(weightset)
    {
      header("vcsn/weights/" + t.get_type() + ".hh");
      os_ << "vcsn::" << t.get_type();
    }

    DEFINE(wordset)
    {
      (void) t;
      header("vcsn/ctx/law_char.hh");
      os_ << "vcsn::wordset<vcsn::set_alphabet<vcsn::char_letters>>";
    }

    DEFINE(other)
    {
      os_ << t.get_type();
    }

    DEFINE(polynomialset)
    {
      os_ << "vcsn::polynomialset<" << incendl;
      t.get_content()->accept(*this);
      os_ << decendl << '>';
      header("vcsn/weights/polynomialset.hh");
    }
#undef DEFINE
  }
}
