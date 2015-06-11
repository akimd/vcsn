#include <vcsn/dyn/context-printer.hh>

#include <vcsn/dyn/type-ast.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>


namespace vcsn
{
  namespace ast
  {
    void context_printer::header(const std::string& h)
    {
      headers_.insert(h);
    }

    void context_printer::linkflags(const std::string& flags)
    {
      // We rely on this initial space when not empty.
      linkflags_ += ' ';
      linkflags_ += flags;
    }

    const std::string& context_printer::linkflags() const
    {
      return linkflags_;
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
      auto type = t.get_type();
      if (type == "delay_automaton")
        header("vcsn/algos/is-synchronized.hh");
      else if (type == "determinized_automaton")
        header("vcsn/algos/determinize.hh");
      else if (type == "detweighted_automaton")
        header("vcsn/algos/determinize.hh");
      else if (type == "expression_automaton")
        header("vcsn/core/expression-automaton.hh");
      else if (type == "filter_automaton")
        header("vcsn/algos/filter.hh");
      else if (type == "focus_automaton")
        header("vcsn/algos/focus.hh");
      else if (type == "mutable_automaton")
        header("vcsn/core/mutable-automaton.hh");
      else if (type == "pair_automaton")
        header("vcsn/algos/synchronizing-word.hh");
      else if (type == "partition_automaton")
        header("vcsn/core/partition-automaton.hh");
      else if (type == "product_automaton")
        header("vcsn/algos/conjunction.hh");
      else if (type == "permutation_automaton")
        header("vcsn/core/permutation-automaton.hh");
      else if (type == "scc_automaton")
        header("vcsn/algos/scc.hh");
      else if (type == "synchronized_automaton")
        header("vcsn/algos/synchronize.hh");
      else if (type == "transpose_automaton")
        header("vcsn/algos/transpose.hh");
      else if (type == "tuple_automaton")
        header("vcsn/core/tuple-automaton.hh");
      else
        raise("unsupported automaton type: ", type);

      os_ << "vcsn::" << type << '<' << incendl;
      bool first = true;
      for (auto c: t.get_content())
        {
          if (!first)
            os_ << ',' << iendl;
          first = false;
          c->accept(*this);
        }
      os_ << decendl << '>';
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
      bool first = true;
      for (auto v: t.get_sets())
      {
        if (!first)
          os_ << ',' << iendl;
        first = false;
        v->accept(*this);
      }
      os_ << decendl << '>';
    }


    DEFINE(nullableset)
    {
      header("vcsn/labelset/nullableset.hh");
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

    DEFINE(genset)
    {
      header("vcsn/alphabets/setalpha.hh"); // set_alphabet
      if (t.letter_type() == "char_letters")
        header("vcsn/alphabets/char.hh");
      os_ << "vcsn::set_alphabet<vcsn::" << t.letter_type() << '>';
    }

    DEFINE(letterset)
    {
      header("vcsn/labelset/letterset.hh");
      os_ << "vcsn::letterset<";
      t.genset()->accept(*this);
      os_ << '>';
    }

    DEFINE(expressionset)
    {
      os_ << "vcsn::expressionset<" << incendl;
      t.get_context()->accept(*this);
      os_ << decendl << '>';
      header("vcsn/core/rat/expressionset.hh");
    }

    DEFINE(weightset)
    {
      header("vcsn/weightset/" + t.get_type() + ".hh");
      if (t.get_type() == "qmp")
        linkflags("-lgmp -lgmpxx");
      os_ << "vcsn::" << t.get_type();
    }

    DEFINE(wordset)
    {
      header("vcsn/labelset/wordset.hh");
      os_ << "vcsn::wordset<";
      t.genset()->accept(*this);
      os_ << '>';
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
