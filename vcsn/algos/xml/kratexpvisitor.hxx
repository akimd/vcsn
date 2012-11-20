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

# define VISIT(Type, Name)                                              \
    template <typename Context>                                         \
    inline                                                              \
    void                                                                \
    xml_kratexp_visitor<Context>::visit(const Type##_t& Name)

    VISIT(prod, v)
    {
      print_weight(v, root_);
      auto mul = details::create_node(doc_, "mul");
      root_.appendChild(mul);
      for (auto t : v)
        {
          this_t child(doc_, *mul, ctx_);
          t->accept(child);
        }
    }

    VISIT(sum, v)
    {
      print_weight(v, root_);
      auto plus = details::create_node(doc_, "plus");
      root_.appendChild(plus);
      for (auto t : v)
        {
          this_t child(doc_, *plus, ctx_);
          t->accept(child);
        }

    }

    VISIT(star, v)
    {
      print_weight(v, root_);
      if (auto sub = v.sub())
        {
          auto star = details::create_node(doc_, "star");
          root_.appendChild(star);
          this_t child(doc_, *star, ctx_);
          sub->accept(child);
        }
    }

    VISIT(one, v)
    {
      print_weight(v, root_);
      auto node = details::create_node(doc_, "one");
      root_.appendChild(node);
    }

    VISIT(zero, v)
    {
      print_weight(v, root_);
      auto node = details::create_node(doc_, "zero");
      root_.appendChild(node);
    }

    VISIT(atom, v)
    {
      auto label = details::create_node(doc_, "atom");

      print_weight(v, *label);

      auto monElmt = details::create_node(doc_, "monElmt");
      auto monGen = details::create_node(doc_, "monGen");

      details::set_attribute(monGen, "value", ctx_.genset()->format(v.value()));
      monElmt->appendChild(monGen);
      label->appendChild(monElmt);
      root_.appendChild(label);
    }

    template <typename Context>
    void
    xml_kratexp_visitor<Context>::print_weight(const node_t& n, dom_elt_t& root)
    {
      if (shows_(n.left_weight()))
        {
          auto weight = details::create_node(doc_, "weight");
          details::set_attribute(weight, "value", ws_->format(n.left_weight()));
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
