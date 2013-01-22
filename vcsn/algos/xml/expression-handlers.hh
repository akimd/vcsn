#ifndef VCSN_ALGOS_XML_EXPRESSION_HANDLERS_HH
# define VCSN_ALGOS_XML_EXPRESSION_HANDLERS_HH

# include <cassert>
# include <iostream>
# include <memory>
# include <stdexcept>
# include <stack>

// FIXME: Clean headers
# include <xercesc/sax2/SAX2XMLReader.hpp>
# include <xercesc/sax2/XMLReaderFactory.hpp>
//# include <xercesc/sax2/DefaultHandler.hpp>
//# include <xercesc/sax2/Attributes.hpp>

# include <xercesc/sax/SAXException.hpp>
# include <xercesc/util/XMLString.hpp>

# include <vcsn/algos/xml/handlers-base.hh>
# include <vcsn/algos/xml/tools.hh>
# include <vcsn/algos/xml/value-type.hh>
# include <vcsn/algos/xml/xml-eq.hh>
# include <vcsn/core/kind.hh>
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/core/rat/ratexpset.hh>
# include <vcsn/empty.hh>

# define EQUAL(NodeName) eq(eq_.NodeName, localname)
# define ATTR_NAME_EQUAL(AttrName, I) eq(eq_.AttrName, attr.getLocalName(I))
# define ATTR_VALUE_EQUAL(AttrName, I) eq(eq_.AttrName, attr.getValue(I))

namespace vcsn
{
  namespace details
  {
    template <typename Context>
    class ExpressionHandler;

    template <typename Context>
    class Label;

    template <typename LabelSet,
              typename WeightSet,
              template <typename, typename, typename> class Context>
    struct Label<Context<LabelSet, WeightSet, labels_are_unit>>
    {
      static empty_t get(const char*)
      {
        return {};
      }
    };

    template <typename LabelSet,
              typename WeightSet,
              template <typename, typename, typename> class Context>
    struct Label<Context<LabelSet, WeightSet, labels_are_letters>>
    {
      static char get(const char* label)
      {
        return *label;
      }
    };

    template <typename LabelSet,
              typename WeightSet,
              template <typename, typename, typename> class Context>
    struct Label<Context<LabelSet, WeightSet, labels_are_words>>
    {
      static const char* get(const char* label)
      {
        return label;
      }
    };

    // Weight reading
    template <typename WeighSet>
    class XMLWeightBehavior;

    // Special case for rational expression weighted context
    template <typename Context>
    class XMLWeightBehavior<std::shared_ptr<vcsn::ratexpset<Context>>>
    {
    public:
      using weightset_t = std::shared_ptr<vcsn::ratexpset<Context>> const;
      using weight_t = typename vcsn::ratexpset<Context>::value_t;
      using string = const XMLCh* const;
      using attributes = const xercesc::Attributes&;
      using parser_t = xercesc::SAX2XMLReader;

      XMLWeightBehavior(parser_t& parser,
                        Handler& father,
                        const weightset_t& ws)
        : p_(parser)
        , f_(father)
        , ws_(ws)
        , weight_expr_(ws_.context(), parser)
      {}

      void set(string localname, attributes attrs)
      {
        p_.setContentHandler(&f_);
      }

      void get()
      {
        return weight_expr_;
      }

    private:
      weightset_t ws_;
      weight_t value_;
      parser_t p_;
      Handler& f_;
      ExpressionHandler<Context> weight_expr_;
    };

    // General case for weight reading
    template <typename WeightSet>
    class XMLWeightBehavior
    {
    public:
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      using parser_t = xercesc::SAX2XMLReader;
      using string = const XMLCh* const;
      using attributes = const xercesc::Attributes&;

      XMLWeightBehavior(parser_t&, const Handler&, const weightset_t& ws)
        : ws_(ws)
      {}

      void set(string localname, attributes attrs)
      {
        if (attrs.getLength() != 1)
          throw std::domain_error("Bad number of attributes for weight node.");
        if (EQUAL(value))
          throw std::domain_error("Bad attribute name for weight node.");
        value_ = ws_.conv(transcode(attrs.getValue(XMLSize_t())));
      }

      weight_t get() const
      {
        return value_;
      }

    private:
      weight_t value_;
      weightset_t ws_;
      xmlEq eq_; // FIXME: Use singleton
    };

    // Handler for expression structure This class is used for the
    // following class named StaticContentHandler. Must inherits
    // vcsn::details::Handler.
    //
    // Must implements the function type. This function as the
    // following prototype: static XMLCh* type().
    //
    // Must implements the function get_parsed_content().
    //
    // Must define the type content_t.
    //
    template <typename Context>
    class ExpressionHandler : public Handler
    {
    public:
      using context_t = Context;
      using content_t = vcsn::rat::exp_t;
      using label_t = typename Context::label_t;
      using weightset_t = typename context_t::weightset_t;
      using weight_t = typename context_t::weight_t;
      using stack_elt = vcsn::rat::inner<label_t, weight_t>;
      using ratexpset_t = ratexpset<context_t>;
      using value_t = typename ratexpset_t::value_t;

      ExpressionHandler(const context_t& ctx, parser_t& parser, Handler& father)
        : Handler(parser)
        , ratexpset_(ctx)
        , ctx_(ctx)
        , weight_behavior_(parser, *this, *ctx_.weightset())
        , father_(father)
      {}

      static std::string type()
      {
        return "expression";
      }

      virtual void start(string,
                         string localname,
                         string,
                         attributes attr) override final
      {
        if (EQUAL(plus))
          count_.push(0);
        else if (EQUAL(product))
          count_.push(0);
        else if (EQUAL(star))
          count_.push(0);
        else if (EQUAL(label))
          {
            if (attr.getLength() != 1)
              throw std::domain_error("Invalid number of attribute for"
                                      " atom node.");
            if (!ATTR_NAME_EQUAL(atom, 0))
              throw std::domain_error("Invalid name for atom node attribute.");

            stack_
              .push(ratexpset_
                    .atom(Label<context_t>
                          ::get(transcode(attr.getValue(XMLSize_t())))));

            ++count_.top();
          }
        else if (EQUAL(zero))
          {
            if (attr.getLength() != 0)
              throw std::domain_error("Invalid number of attribute for "
                                      "zero node.");
            stack_.push(ratexpset_.zero());
            ++count_.top();
          }
        else if (EQUAL(one))
          {
            if (attr.getLength() != 0)
              throw std::domain_error("Invalid number of attribute for "
                                      "one node.");
            stack_.push(ratexpset_.unit());
            ++count_.top();
          }
        else if (EQUAL(weight))
          weight_behavior_.set(localname, attr);
        else
          throw std::domain_error(std::string("Invalid ")
                                  + transcode(localname)
                                  + " node in expression.");
      }

      virtual void end(string, string localname, string) override final
      {
        if (EQUAL(plus))
          {
            // FIXME: We are right associative. We want to be left associative.
            unsigned count = count_.top();
            count_.pop();
            assert(count <= stack_.size());

            while (count > 1)
              {
                auto right = stack_.top();
                stack_.pop();
                auto left = stack_.top();

                stack_.pop();
                stack_.push(ratexpset_.add(left, right));
                --count;
              }
            if (count_.empty())
              parser_.setContentHandler(&father_);
            else
              ++count_.top();
          }
        else if (EQUAL(product))
          {
            // FIXME: We are right associative. We want to be left associative.
            unsigned count = count_.top();
            count_.pop();
            assert(count <= stack_.size());

            while (count > 1)
              {
                auto right = stack_.top();
                stack_.pop();
                auto left = stack_.top();

                stack_.pop();
                stack_.push(ratexpset_.mul(left, right));
              }
            if (count_.empty())
              parser_.setContentHandler(&father_);
            else
              ++count_.top();
          }
        else if (EQUAL(star))
          {
            assert(!stack_.empty());
            if (count_.top() != 1)
              throw std::domain_error("The node star must have only one son.");

            auto elt = stack_.top();

            stack_.pop();
            stack_.push(ratexpset_.star(elt));
            if (count_.empty())
              parser_.setContentHandler(&father_);
            else
              ++count_.top();
          }
        else if (EQUAL(weight))
          {
            auto elt = stack_.top();
            stack_.pop();
            stack_.push(ratexpset_.weight(weight_behavior_.get(), elt));
            // FIXME: We have only left weight.
          }
#ifndef NDEBUG
        else if (!(EQUAL(one) || EQUAL(zero) || EQUAL(label)))
          throw std::domain_error(std::string("Unknown node ")
                                  + transcode(localname) + ".");
#endif // !NDEBUG
      }

      content_t get_parsed_content()
      {
        assert(stack_.size() == 1);
        return stack_.top();
      }

    private:
      ratexpset_t ratexpset_;
      const context_t& ctx_;
      XMLWeightBehavior<weightset_t> weight_behavior_;
      Handler& father_;
      std::stack<value_t> stack_;
      std::stack<unsigned> count_;
    };

  } // namespace details
} // namespace vcsn

# undef ATTR_NAME_EQUAL
# undef ATTR_VALUE_EQUAL
# undef EQUAL

#endif // !VCSN_ALGOS_XML_EXPRESSION_HANDLERS_HH
