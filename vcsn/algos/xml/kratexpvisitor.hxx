#ifndef VCSN_ALGOS_XML_KRATEXPVISITOR_HXX
# define VCSN_ALGOS_XML_KRATEXPVISITOR_HXX

# include <xercesc/dom/DOM.hpp>
# include <vcsn/algos/xml/tools.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename Context>
    xml_kratexp_visitor<Context>::xml_kratexp_visitor(xercesc::DOMDocument& doc,
                                                      xercesc::DOMElement& root,
                                                      const context_t& ctx)
      : doc_(doc)
      , root_(root)
      , ctx_(ctx)
      , ws_(ctx.weightset())
    {}

    template <typename Context>
    xml_kratexp_visitor<Context>::~xml_kratexp_visitor()
    {}


    template <typename Context>
    void
    xml_kratexp_visitor<Context>::operator()(const node_t& v)
    {
      v.accept(*this);
    }

    template <typename Context>
    void
    xml_kratexp_visitor<Context>::operator()(const node_ptr& v)
    {
      operator()(*v);
    }

# define VISIT(Type, Name)                                      \
    template <typename Context>                                 \
    inline                                                      \
    void                                                        \
    xml_kratexp_visitor<Context>::visit(const Type##_t& Name)

    VISIT(prod, v)
    {
      auto prod = details::create_node(doc_, "prod");
      root_.appendChild(prod);
      print_weight(v.left_weight(), *prod, "left");
      for (auto t : v)
        {
          this_t child(doc_, *prod, ctx_);
          t->accept(child);
        }
      print_weight(v.right_weight(), *prod, "right");
    }

    VISIT(sum, v)
    {
      auto plus = details::create_node(doc_, "plus");
      root_.appendChild(plus);
      print_weight(v.left_weight(), *plus, "left");
      for (auto t : v)
        {
          this_t child(doc_, *plus, ctx_);
          t->accept(child);
        }
      print_weight(v.right_weight(), *plus, "right");
    }

    VISIT(star, v)
    {
      if (auto sub = v.sub())
        {
          auto star = details::create_node(doc_, "star");
          root_.appendChild(star);
          print_weight(v.left_weight(), *star, "left");
          this_t child(doc_, *star, ctx_);
          sub->accept(child);
          print_weight(v.left_weight(), *star, "right");
        }
    }

    VISIT(one, v)
    {
      auto one = details::create_node(doc_, "one");
      root_.appendChild(one);
      print_weight(v.left_weight(), *one, "left");
    }

    VISIT(zero, v)
    {
      auto zero = details::create_node(doc_, "zero");
      root_.appendChild(zero);
      print_weight(v.left_weight(), *zero, "left");
    }

    VISIT(atom, v)
    {
      auto atom = details::create_node(doc_, "atom");
      root_.appendChild(atom);

      print_weight(v.left_weight(), *atom, "left");

      auto label = details::create_node(doc_, "label");
      atom->appendChild(label);

      details::set_attribute(label, "value",
                             ctx_.genset()->format(v.value()));
    }

    template <typename Context>
    void
    xml_kratexp_visitor<Context>::print_weight(const weight_t& w,
                                               dom_elt_t& root,
                                               const std::string& side)
    {
      if (shows_(w))
        {
          assert(side == "left" || side == "right");
          auto weight = details::create_node(doc_, side + "Weight");
          details::set_attribute(weight, "value", ws_->format(w));
          root.appendChild(weight);
        }
    }

    template <typename Context>
    bool
    xml_kratexp_visitor<Context>::shows_(const weight_t& w) const
    {
      return ws_->show_unit() || !ws_->is_unit(w);
    }

# undef VISIT

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_ALGOS_XML_KRATEXPVISITOR_HXX
