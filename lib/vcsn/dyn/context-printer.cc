#include <vcsn/dyn/context-printer.hh>
#include <vcsn/dyn/type-ast.hh>

#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>


namespace vcsn
{
  namespace ast
  {

    void context_printer::header(std::string h)
    {
      // Open code some mismatches between algo name, and header
      // name.  FIXME: algorithms should register this themselves.
      if (false) {}
#define ALGO(In, Out)                       \
      else if (h == "vcsn/algos/" In ".hh") \
        h = "vcsn/algos/" Out ".hh"
      ALGO("chain_ratexp", "concatenate");
      ALGO("concatenate_ratexp", "concatenate");
      ALGO("copy_ratexp", "copy");
      ALGO("infiltration", "product");
      ALGO("info_ratexp", "info");
      ALGO("intersection_ratexp", "product");
      ALGO("is_valid_ratexp", "is-valid");
      ALGO("left_mult", "left-mult");
      ALGO("left_mult_ratexp", "left-mult");
      ALGO("list_ratexp", "print");
      ALGO("make_context", "make-context");
      ALGO("print_ratexp", "print");
      ALGO("print_weight", "print");
      ALGO("product3", "product");
      ALGO("right_mult", "left-mult");
      ALGO("right_mult_ratexp", "left-mult");
      ALGO("shuffle", "product");
      ALGO("standard_ratexp", "standard");
      ALGO("sum_ratexp", "sum");
      ALGO("transpose_ratexp", "transpose");
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
      if (t.get_type() == "mutable_automaton")
      {
        os_ << "vcsn::mutable_automaton<" << incendl;
        t.get_content()->accept(*this);
        os_ << decendl << '>';
        header("vcsn/core/mutable_automaton.hh");
      }
      else if (t.get_type() == "transpose_automaton")
      {
        os_ << "vcsn::detail::transpose_automaton<" << incendl;
        t.get_content()->accept(*this);
        os_ << decendl << '>';
        header("vcsn/algos/transpose.hh");
      }
      else
        raise("unsupported automaton type: ", t.get_type());
    }

    DEFINE(context)
    {
      header("vcsn/ctx/context.hh");
      os_ << "vcsn::context<" << incendl;
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
      os_ << "vcsn::oneset";
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
      header("vcsn/weightset/" + t.get_type() + ".hh");
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
      header("vcsn/weightset/polynomialset.hh");
    }
#undef DEFINE
  }
}
