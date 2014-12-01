#include <vcsn/dyn/context-printer.hh>

#include <boost/algorithm/string.hpp>

#include <vcsn/dyn/type-ast.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/raise.hh>


namespace vcsn
{
  namespace ast
  {

    void context_printer::header_algo(const std::string& algo)
    {
      // We use '-' instead of '_' in header file names.
      auto h = boost::replace_all_copy(algo, "_", "-");
      // We don't use any suffix in the file names.
      for (auto s: {"-automaton",
                    "-ctx",
                    "-expansion", "-label", "-polynomial",
                    "-expression", "-vector", "-weight"})
        if (boost::ends_with(h, s) && h != "to-expression")
          boost::erase_tail(h, strlen(s));
      // Open code some mismatches between algo name, and header name.
      //
      // FIXME: algorithms should register this themselves.
      if (false) {}
#define ALGO(In, Out)                           \
      else if (h == In)                         \
        h = Out
      ALGO("ambiguous-word", "is-ambiguous");
      ALGO("chain", "concatenate");
      ALGO("coaccessible", "accessible");
      ALGO("codeterminize", "determinize");
      ALGO("cominimize", "minimize");
      ALGO("component", "scc");
      ALGO("condense", "scc");
      ALGO("conjunction", "product");
      ALGO("context-of", "make-context");
      ALGO("copy-convert", "copy");
      ALGO("costandard", "standard");
      ALGO("difference", "are-equivalent");
      ALGO("eliminate-state", "to-expression");
      ALGO("factor", "prefix");
      ALGO("infiltration", "product");
      ALGO("is-accessible", "accessible");
      ALGO("is-coaccessible", "accessible");
      ALGO("is-codeterministic", "is-deterministic");
      ALGO("is-costandard", "standard");
      ALGO("is-cycle-ambiguous", "is-ambiguous");
      ALGO("is-empty", "accessible");
      ALGO("is-normalized", "normalize");
      ALGO("is-out-sorted", "sort");
      ALGO("is-standard", "standard");
      ALGO("is-synchronized-by", "synchronizing-word");
      ALGO("is-trim", "accessible");
      ALGO("is-useless", "accessible");
      ALGO("ldiv", "divide");
      ALGO("lgcd", "divide");
      ALGO("list", "print");
      ALGO("make-expressionset", "make-context");
      ALGO("make-word-context", "make-context");
      ALGO("multiply", "concatenate");
      ALGO("num-components", "scc");
      ALGO("pair", "synchronizing-word");
      ALGO("power", "product");
      ALGO("rdiv", "divide");
      ALGO("right-mult", "left-mult");
      ALGO("shortest", "enumerate");
      ALGO("shuffle", "product");
      ALGO("subword", "prefix");
      ALGO("suffix", "prefix");
      ALGO("trim", "accessible");
      ALGO("union-a", "union");
#undef ALGO
      // Exceptions.
      if (algo == "is_valid_expression")
        h = "is-valid-expression";

      h = "vcsn/algos/" + h + ".hh";
      headers_late_.insert(h);
    }

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
      if (type == "focus_automaton")
        header("vcsn/algos/focus.hh");
      else if (type == "determinized_automaton")
        header("vcsn/algos/determinize.hh");
      else if (type == "detweighted_automaton")
        header("vcsn/algos/determinize.hh");
      else if (type == "expression_automaton")
        header("vcsn/core/expression-automaton.hh");
      else if (type == "filter_automaton")
        header("vcsn/algos/filter.hh");
      else if (type == "mutable_automaton")
        header("vcsn/core/mutable-automaton.hh");
      else if (type == "pair_automaton")
        header("vcsn/algos/synchronizing-word.hh");
      else if (type == "partition_automaton")
        header("vcsn/core/partition-automaton.hh");
      else if (type == "product_automaton")
        header("vcsn/algos/product.hh");
      else if (type == "permutation_automaton")
        header("vcsn/core/permutation-automaton.hh");
      else if (type == "scc_automaton")
        header("vcsn/algos/scc.hh");
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
