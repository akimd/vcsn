#pragma once

#include <iostream>

#include <vcsn/algos/project.hh> // bad layering: should not be in algos.
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/misc/algorithm.hh> // transform
#include <vcsn/misc/format.hh>
#include <vcsn/misc/indent.hh>

namespace vcsn
{
  namespace rat
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

      /// Set output format.
      void format(format fmt)
      {
        fmt_ = fmt;
        if (fmt_ == format::latex)
          {
            star_          = "^{*}";
            complement_    = "^{c}";
            transposition_ = "^{T}";
            conjunction_   = " \\& ";
            infiltration_  = " \\uparrow ";
            shuffle_       = " \\between ";
            product_       = " \\, ";
            sum_           = (rs_.identities().is_distributive() ? " \\oplus "
                              : " + ");
            zero_          = "\\emptyset";
            one_           = "\\varepsilon";
            lmul_          = "\\,";
            rmul_          = "\\,";
            ldiv_          = " \\backslash ";
            tuple_         = " \\mid ";
          }
        else if (fmt_ == format::text)
          {
            star_          = "*";
            complement_    = "{c}";
            transposition_ = "{T}";
            conjunction_   = "&";
            infiltration_  = "&:";
            shuffle_       = ":";
            product_       = ".";
            sum_           = "+";
            zero_          = "\\z";
            one_           = "\\e";
            lmul_          = "<.>.";
            rmul_          = ".<.>";
            ldiv_          = "{\\}";
            tuple_         = "|";
          }
        else if (fmt_ == format::utf8)
          {
            star_          = "*";
            complement_    = "ᶜ";
            transposition_ = "ᵗ";
            conjunction_   = "&";
            infiltration_  = "&:";
            shuffle_       = ":";
            product_       = ".";
            sum_           = "+";
            zero_          = "∅";
            one_           = "ε";
            lmul_          = "⟨.⟩.";
            rmul_          = ".⟨.⟩";
            ldiv_          = "{\\}";
            tuple_         = "|";
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
        print(v);
        out_ << vcsn::decendl
             << "}";
        return out_;
      }

      /// Print an expression as a tree.
      std::ostream& print(const value_t& v)
      {
        print_(v);
        return out_;
      }

    private:
      using name_t = unsigned;

      /// Easy recursion: print an expression and return its ID.
      name_t print_(const value_t& v)
      {
        return print_(*v);
      }

      /// Easy recursion: print an expression and return its ID.
      name_t print_(const node_t& v)
      {
        v.accept(*this);
        return last_name_;
      }

      VCSN_RAT_VISIT(atom, v)          { print_(v); }
      VCSN_RAT_VISIT(complement, v)    { print_(v, complement_); }
      VCSN_RAT_VISIT(conjunction, v)   { print_(v, conjunction_); }
      VCSN_RAT_VISIT(infiltration, v)  { print_(v, infiltration_); }
      VCSN_RAT_VISIT(ldiv, v)          { print_(v, ldiv_); }
      VCSN_RAT_VISIT(lweight, v)       { print_(v); }
      VCSN_RAT_VISIT(one, v)           { print_(v, one_); }
      VCSN_RAT_VISIT(prod, v)          { print_(v, product_); }
      VCSN_RAT_VISIT(rweight, v)       { print_(v); }
      VCSN_RAT_VISIT(shuffle, v)       { print_(v, shuffle_); }
      VCSN_RAT_VISIT(star, v)          { print_(v, star_); }
      VCSN_RAT_VISIT(sum, v)           { print_(v, sum_); }
      VCSN_RAT_VISIT(transposition, v) { print_(v, transposition_); }
      VCSN_RAT_VISIT(zero, v)          { print_(v, zero_); }

      using tuple_t = typename super_t::tuple_t;

      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// Print one tape.
        template <size_t I>
        void print_(const tuple_t& v)
        {
          auto rs = detail::project<I>(self_.rs_);
          const auto& r = std::get<I>(v.sub());
          self_.out_
            << vcsn::iendl
            << address(v) << " -> "
            << address(*r) << vcsn::iendl;
          auto printer = make_dot_printer(rs, self_.out_);
          printer.format(self_.fmt_);
          printer.print(r);
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
          self_.out_ << address(v)
                     << " [label=\"" << self_.tuple_ << "\"]";
          print_(v, labelset_t::indices);
        }
        const self_t& self_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        void operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        const self_t& self_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        visit_tuple<>{*this}(v);
      }

      /// The identifier for this node.
      template <typename Node>
      name_t name_(const Node& n)
      {
        if (physical_)
          {
            auto p = names_.emplace(&n, 0);
            if (p.second)
              p.first->second = names_.size();
            return p.first->second;
          }
        else
          return count_++;
      }

      /// Print a nullary node.
      template <rat::exp::type_t Type>
      void print_(const constant_t<Type>& n, const char* op)
      {
        auto name = name_(n);
        out_ << name
             << " [label=\"" << op << "\"]";
        last_name_ = name;
      }

      /// Print a unary node.
      template <rat::exp::type_t Type>
      void print_(const unary_t<Type>& n, const char* op)
      {
        auto name = name_(n);
        auto sub = print_(n.sub());
        out_ << vcsn::iendl
             << name << " [label=\"" << op << "\"]"
             << vcsn::iendl
             << name << " -> " << sub;
        last_name_ = name;
      }

      /// Print a variadic node.
      template <rat::exp::type_t Type>
      void print_(const variadic_t<Type>& n, const char* op)
      {
        auto name = name_(n);
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
        last_name_ = name;
      }

      /// Print a weight.
      name_t print_(const weight_t& w)
      {
        auto res = name_(w);
        out_ << vcsn::iendl
             << res << " [label=\"";
        rs_.weightset()->print(w, out_, fmt_.for_weights());
        out_ << "\"]";
        return res;
      }

      /// Print a left-weight.
      void print_(const lweight_t& n)
      {
        auto name = name_(n);
        auto weight = print_(n.weight());
        auto sub = print_(n.sub());
        out_ << vcsn::iendl
             << name << " [label=\"" << lmul_ << "\"]" << vcsn::iendl
             << name << " -> " << weight << vcsn::iendl
             << name << " -> " << sub;
        last_name_ = name;
      }

      /// Print a right-weight.
      void print_(const rweight_t& n)
      {
        auto name = name_(n);
        auto sub = print_(n.sub());
        auto weight = print_(n.weight());
        out_ << vcsn::iendl
             << name << " [label=\"" << rmul_ << "\"]" << vcsn::iendl
             << name << " -> " << sub << vcsn::iendl
             << name << " -> " << weight;
        last_name_ = name;
      }

      /// Print a label.
      void print_(const atom_t& n)
      {
        auto name = name_(n);
        out_ << vcsn::iendl;
        out_ << name << " [label=\"";
        rs_.labelset()->print(n.value(), out_, fmt_.for_labels());
        out_ << "\"]";
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
      std::unordered_map<const void*, name_t> names_;

      /// The node counter, used to name the nodes.
      unsigned count_ = 0;
      /// The name of the last visited node.
      name_t last_name_;

      /// External product.
      const char* lmul_ = nullptr;
      const char* rmul_ = nullptr;
      /// Quotient.
      const char* ldiv_ = nullptr;
      /// The expression operators.
      const char* star_ = nullptr;
      const char* complement_ = nullptr;
      const char* transposition_ = nullptr;
      const char* conjunction_ = nullptr;
      const char* infiltration_ = nullptr;
      const char* shuffle_ = nullptr;
      const char* product_ = nullptr;
      const char* sum_ = nullptr;

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
  } // namespace rat
} // namespace vcsn
