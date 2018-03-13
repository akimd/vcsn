#pragma once

#include <iostream>
#include <memory>

#include <vcsn/algos/project.hh> // bad layering: should not be in algos.
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/algorithm.hh> // transform
#include <vcsn/misc/format.hh>
#include <vcsn/misc/indent.hh>

namespace vcsn::rat
{
  /// Pretty-printer for rational expressions.
  template <typename ExpSet>
  class dot_printer
    : public ExpSet::const_visitor
  {
  public:
    using expressionset_t = ExpSet;
    using super_t = typename expressionset_t::const_visitor;
    using self_t = dot_printer;

    using context_t = context_t_of<expressionset_t>;
    using identities_t = typename expressionset_t::identities_t;
    using labelset_t = labelset_t_of<context_t>;
    using label_t = label_t_of<context_t>;
    using weight_t = weight_t_of<context_t>;

    /// Actual node, without indirection.
    using node_t = typename super_t::node_t;
    /// A shared_ptr to node_t.
    using value_t = typename node_t::value_t;
    template <type_t Type>
    using constant_t = typename super_t::template constant_t<Type>;
    template <type_t Type>
    using unary_t = typename super_t::template unary_t<Type>;
    template <type_t Type>
    using variadic_t = typename super_t::template variadic_t<Type>;

    /// Name of this algorithm, for error messages.
    constexpr static const char* me() { return "dot_printer"; }

    /// A printer.
    ///
    /// \param rs   the expressionset
    /// \param out  the output stream
    /// \param physical  whether to display the physical DAG
    ///             rather than the logical tree.
    dot_printer(const expressionset_t& rs, std::ostream& out,
                bool physical = false)
      : out_{out}
      , rs_{rs}
      , physical_{physical}
    {
      format(vcsn::format("utf8"));
    }

    /// Construct from another dot_printer.
    ///
    /// Used to "fork" new printers in the case of tuples, with
    /// sharing of the state.
    template <typename OtherExpSet>
    dot_printer(const expressionset_t& rs,
                dot_printer<OtherExpSet>& other)
      : out_{other.out_}
      , rs_{rs}
      , physical_{other.physical_}
      , names_{other.names_}
      , count_{other.count_}
    {
      format(other.fmt_);
    }

    /// Make it possible to view members from dot_printer's with a
    /// different template parameter.
    template <typename OtherExpSet>
    friend class dot_printer;

    /// Set output format.
    void format(format fmt)
    {
      fmt_ = fmt;
      if (fmt_ == format::latex)
        {
          add_           = (rs_.identities().is_distributive() ? " \\oplus "
                            : " + ");
          complement_    = "^{c}";
          compose_       = "@";
          conjunction_   = " \\& ";
          infiltrate_    = " \\uparrow ";
          ldivide_       = " \\backslash ";
          lweight_       = "\\,";
          one_           = "\\varepsilon";
          mul_           = " \\, ";
          rweight_       = "\\,";
          shuffle_       = " \\between ";
          star_          = "^{*}";
          transposition_ = "^{T}";
          tuple_         = " \\mid ";
          zero_          = "\\emptyset";
        }
      else if (fmt_ == format::text)
        {
          add_           = "+";
          complement_    = "{c}";
          compose_       = "@";
          conjunction_   = "&";
          infiltrate_    = "&:";
          ldivide_       = "{\\}";
          lweight_       = "<.>.";
          one_           = "\\e";
          mul_           = ".";
          rweight_       = ".<.>";
          shuffle_       = ":";
          star_          = "*";
          transposition_ = "{T}";
          tuple_         = "|";
          zero_          = "\\z";
        }
      else if (fmt_ == format::utf8)
        {
          add_           = "+";
          complement_    = "ᶜ";
          compose_       = "@";
          conjunction_   = "&";
          infiltrate_    = "&:";
          ldivide_       = "{\\}";
          lweight_       = "⟨.⟩.";
          one_           = "ε";
          mul_           = ".";
          rweight_       = ".⟨.⟩";
          shuffle_       = ":";
          star_          = "*";
          transposition_ = "ᵗ";
          tuple_         = "|";
          zero_          = "∅";
        }
      else
        raise("expression: invalid format: ", fmt_);
    }

    /// Entry point: print \a v as a complete Dot graph.
    std::ostream& operator()(const value_t& v)
    {
      out_ << "digraph" << vcsn::iendl
           << "{" << vcsn::incendl
           << "edge [arrowhead = vee, arrowsize = .6]" << vcsn::iendl
           << "node [shape = circle, style = rounded, width = 0.5]\n";
      print_(v);
      out_ << vcsn::decendl
           << "}";
      return out_;
    }

  private:
    /// A type to label the node.
    using id_t = unsigned;

    /// Easy recursion: print an expression and return its ID.
    id_t print_(const value_t& v)
    {
      return print_(*v);
    }

    /// Easy recursion: print an expression and return its ID.
    id_t print_(const node_t& v)
    {
      v.accept(*this);
      return last_name_;
    }

    VCSN_RAT_VISIT(add, v)           { print_(v, add_); }
    VCSN_RAT_VISIT(atom, v)          { print_(v); }
    VCSN_RAT_VISIT(complement, v)    { print_(v, complement_); }
    VCSN_RAT_VISIT(compose, v)       { print_(v, compose_); }
    VCSN_RAT_VISIT(conjunction, v)   { print_(v, conjunction_); }
    VCSN_RAT_VISIT(infiltrate, v)    { print_(v, infiltrate_); }
    VCSN_RAT_VISIT(ldivide, v)       { print_(v, ldivide_); }
    VCSN_RAT_VISIT(lweight, v)       { print_(v); }
    VCSN_RAT_VISIT(mul, v)           { print_(v, mul_); }
    VCSN_RAT_VISIT(name, v)          { print_(v); }
    VCSN_RAT_VISIT(one, v)           { print_(v, one_); }
    VCSN_RAT_VISIT(rweight, v)       { print_(v); }
    VCSN_RAT_VISIT(shuffle, v)       { print_(v, shuffle_); }
    VCSN_RAT_VISIT(star, v)          { print_(v, star_); }
    VCSN_RAT_VISIT(transposition, v) { print_(v, transposition_); }
    VCSN_RAT_VISIT(zero, v)          { print_(v, zero_); }

    using tuple_t = typename super_t::tuple_t;

    template <typename = void>
    struct visit_tuple
    {
      /// Print one tape.
      template <size_t I>
      void print_(const tuple_t& v)
      {
        auto rs = detail::project<I>(self_.rs_);
        const auto& r = std::get<I>(v.sub());
        auto printer = dot_printer<decltype(rs)>(rs, self_);
        auto name = printer.print_(r);
        self_.out_ << vcsn::iendl
                   << name_ << " -> " << name;
      }

      /// Print all the tapes.
      template <size_t... I>
      void print_(const tuple_t& v, detail::index_sequence<I...>)
      {
        using swallow = int[];
        (void) swallow
        {
          (print_<I>(v),
           0)...
        };
      }

      /// Entry point.
      void operator()(const tuple_t& v)
      {
        auto [name, traverse] = self_.name_(v);
        if (traverse)
          {
            name_ = name;
            self_.out_ << vcsn::iendl
                       << name_
                       << " [label=\"" << self_.tuple_ << "\"]";
            print_(v, labelset_t::indices);
          }
        self_.last_name_ = name;
      }
      self_t& self_;
      /// The name of the tuple node.
      id_t name_ = {};
    };

    void visit(const tuple_t& v, std::true_type) override
    {
      if constexpr (context_t::is_lat)
        visit_tuple<decltype(v)>{*this}(v);
    }

    /// The identifier for this node, and a Boolean stating whether
    /// we should traverse (again) this node.
    template <typename Node>
    std::pair<id_t, bool> name_(const Node& n)
    {
      if (physical_)
        {
          auto p = names_->emplace(&n, 0);
          if (p.second)
            p.first->second = names_->size();
          return {p.first->second, p.second};
        }
      else
        return {(*count_)++, true};
    }

    /// Print a name node.
    void print_(const name_t& n)
    {
      auto [name, traverse] = name_(n);
      if (traverse)
        {
          auto sub = print_(n.sub());
          out_ << vcsn::iendl
               << name << " [label=\"name=" << n.name_get() << "\"]"
               << vcsn::iendl
               << name << " -> " << sub;
        }
      last_name_ = name;
    }

    /// Print a nullary node.
    template <rat::exp::type_t Type>
    void print_(const constant_t<Type>& n, const char* op)
    {
      auto [name, traverse] = name_(n);
      if (traverse)
        out_ << vcsn::iendl
             << name << " [label=\"" << op << "\"]";
      last_name_ = name;
    }

    /// Print a unary node.
    template <rat::exp::type_t Type>
    void print_(const unary_t<Type>& n, const char* op)
    {
      auto [name, traverse] = name_(n);
      if (traverse)
        {
          auto sub = print_(n.sub());
          out_ << vcsn::iendl
               << name << " [label=\"" << op << "\"]"
               << vcsn::iendl
               << name << " -> " << sub;
        }
      last_name_ = name;
    }

    /// Print a variadic node.
    template <rat::exp::type_t Type>
    void print_(const variadic_t<Type>& n, const char* op)
    {
      auto [name, traverse] = name_(n);
      if (traverse)
        {
          auto subs
            = vcsn::detail::transform(n,
                                      [this](const auto& i)
                                      {
                                        return this->print_(i);
                                      }) ;
          out_<< vcsn::iendl
              << name << " [label=\"" << op << "\"]";
          for (auto s: subs)
            out_ << vcsn::iendl
                 << name << " -> " << s;
        }
      last_name_ = name;
    }

    /// Print a weight.
    id_t print_(const weight_t& w)
    {
      auto res = name_(w).first;
      out_ << vcsn::iendl
           << res << " [label=\"";
      rs_.weightset()->print(w, out_, fmt_.for_weights());
      out_ << "\"]";
      return res;
    }

    /// Print a left-weight.
    void print_(const lweight_t& n)
    {
      auto [name, traverse] = name_(n);
      if (traverse)
        {
          auto weight = print_(n.weight());
          auto sub = print_(n.sub());
          out_
            << vcsn::iendl
            << name << " [label=\"" << lweight_ << "\"]" << vcsn::iendl
            << name << " -> " << weight << vcsn::iendl
            << name << " -> " << sub;
        }
      last_name_ = name;
    }

    /// Print a right-weight.
    void print_(const rweight_t& n)
    {
      auto [name, traverse] = name_(n);
      if (traverse)
        {
          auto sub = print_(n.sub());
          auto weight = print_(n.weight());
          out_
            << vcsn::iendl
            << name << " [label=\"" << rweight_ << "\"]" << vcsn::iendl
            << name << " -> " << sub << vcsn::iendl
            << name << " -> " << weight;
        }
      last_name_ = name;
    }

    /// Print a label.
    void print_(const atom_t& n)
    {
      auto [name, traverse] = name_(n);
      if (traverse)
        {
          out_ << vcsn::iendl
               << name << " [label=\"";
          rs_.labelset()->print(n.value(), out_, fmt_.for_labels());
          out_ << "\"]";
        }
      last_name_ = name;
    }

    /// Output stream.
    std::ostream& out_;
    /// Output format.
    class format fmt_;
    /// The expressionset.
    const expressionset_t& rs_;

    /// Whether to display the physical DAG rather than the logical
    /// tree.
    bool physical_ = false;

    /// If physical_ is enabled, register the identifiers of the nodes.
    using names_t = std::unordered_map<const void*, id_t>;
    /// A shared_ptr, to support tuples.
    std::shared_ptr<names_t> names_ = std::make_shared<names_t>();

    /// The node counter, used to name the nodes.
    /// A shared_ptr, to support tuples.
    std::shared_ptr<unsigned> count_ = std::make_shared<unsigned>(0);
    /// The name of the last visited node.
    id_t last_name_;

    /// External product.
    const char* lweight_ = nullptr;
    const char* rweight_ = nullptr;
    /// Quotient.
    const char* ldivide_ = nullptr;
    /// The expression operators.
    const char* add_ = nullptr;
    const char* complement_ = nullptr;
    const char* compose_ = nullptr;
    const char* conjunction_ = nullptr;
    const char* infiltrate_ = nullptr;
    const char* mul_ = nullptr;
    const char* shuffle_ = nullptr;
    const char* star_ = nullptr;
    const char* transposition_ = nullptr;

    /// Tuple tape separator.
    const char* tuple_ = nullptr;

    /// The constants.
    const char* zero_ = nullptr;
    const char* one_ = nullptr;
  };

  template <typename ExpSet>
  dot_printer<ExpSet>
  make_dot_printer(const ExpSet& rs, std::ostream& out,
                   bool physical = false)
  {
    return {rs, out, physical};
  }
}
